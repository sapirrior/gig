#define _POSIX_C_SOURCE 200809L
#include "sync.h"
#include "locator.h"
#include "ui/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define REPO_OWNER "sapirrior"
#define REPO_NAME "gigs"
#define API_URL "https://api.github.com/repos/" REPO_OWNER "/" REPO_NAME "/releases/latest"
#define USER_AGENT "gig-engine/" GIG_VERSION

/**
 * Executes a command and returns its exit status.
 */
static int run_cmd(const char *cmd) {
    FILE *fp = popen(cmd, "w");
    if (!fp) return -1;
    return pclose(fp);
}

/**
 * Fetches standard output from a command.
 * Returns 0 on success, -1 on failure or empty output.
 */
static int fetch_cmd_output(const char *cmd, char *buf, size_t size) {
    FILE *fp = popen(cmd, "r");
    if (!fp) return -1;
    if (fgets(buf, size, fp) == NULL) {
        pclose(fp);
        return -1;
    }
    // Remove trailing newline or carriage return
    buf[strcspn(buf, "\r\n")] = 0;
    int status = pclose(fp);
    return (status == 0 && strlen(buf) > 0) ? 0 : -1;
}

int gig_sync_pages(void) {
    const char *target_base = gig_get_path_string();
    char parent_dir[1024];
    strncpy(parent_dir, target_base, sizeof(parent_dir));
    char *last_slash = strrchr(parent_dir, '/');
    if (last_slash) *last_slash = '\0';

    // 1. Permission Check
    printf("%s[SYNC]%s Verifying environment...\n", GIG_CLR_CMD, GIG_CLR_RESET);
    if (access(parent_dir, W_OK) != 0) {
        printf("%sfatal:%s No write permission for %s (errno: %d).\n", GIG_CLR_ERROR, GIG_CLR_RESET, parent_dir, errno);
#ifndef __TERMUX__
        printf("%shint:%s Try running with 'sudo'.\n", GIG_CLR_WARN, GIG_CLR_RESET);
#endif
        return 1;
    }

    // 2. Dependency Check
    if (run_cmd("command -v curl >/dev/null 2>&1") != 0 || run_cmd("command -v tar >/dev/null 2>&1") != 0) {
        printf("%sfatal:%s Missing dependencies: 'curl' and 'tar' are required for sync.\n", GIG_CLR_ERROR, GIG_CLR_RESET);
        return 1;
    }

    // 3. Fetch Metadata
    printf("%s[SYNC]%s Contacting GitHub API...\n", GIG_CLR_CMD, GIG_CLR_RESET);
    char tag[128] = {0}, url[1024] = {0};
    char cmd[4096];
    
    // Improved grep/cut to be slightly more resilient to JSON spacing
    snprintf(cmd, sizeof(cmd), "curl -sL -H \"User-Agent: %s\" --connect-timeout 10 %s | grep '\"tag_name\":' | sed -E 's/.*\"tag_name\":\\s*\"([^\"]+)\".*/\\1/'", USER_AGENT, API_URL);
    if (fetch_cmd_output(cmd, tag, sizeof(tag)) != 0) {
        printf("%sfatal:%s Could not retrieve release information from GitHub API.\n", GIG_CLR_ERROR, GIG_CLR_RESET);
        return 1;
    }

    // 4. Version Check
    char version_file[1024];
    snprintf(version_file, sizeof(version_file), "%s/version", target_base);
    FILE *vf = fopen(version_file, "r");
    if (vf) {
        char current_tag[128] = {0};
        if (fgets(current_tag, sizeof(current_tag), vf)) {
            current_tag[strcspn(current_tag, "\r\n")] = 0;
            if (strcmp(current_tag, tag) == 0) {
                printf("%s[DONE]%s Documentation is already at the latest version (%s%s%s).\n", 
                       GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_ARG, tag, GIG_CLR_RESET);
                fclose(vf);
                return 0;
            }
        }
        fclose(vf);
    }

    // 5. Download & Extract
    snprintf(cmd, sizeof(cmd), "curl -sL -H \"User-Agent: %s\" --connect-timeout 10 %s | grep '\"tarball_url\":' | head -n 1 | sed -E 's/.*\"tarball_url\":\\s*\"([^\"]+)\".*/\\1/'", USER_AGENT, API_URL);
    if (fetch_cmd_output(cmd, url, sizeof(url)) != 0) {
        printf("%sfatal:%s Could not retrieve download URL.\n", GIG_CLR_ERROR, GIG_CLR_RESET);
        return 1;
    }

    printf("%s[SYNC]%s Synchronizing %s%s%s...\n", GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_ARG, tag, GIG_CLR_RESET);
    
    char tmp_template[1024];
    const char *env_tmp = getenv("TMPDIR");
    if (!env_tmp) {
#ifdef __TERMUX__
        env_tmp = "/data/data/com.termux/files/usr/tmp";
#else
        env_tmp = "/tmp";
#endif
    }
    snprintf(tmp_template, sizeof(tmp_template), "%s/gig_sync_XXXXXX", env_tmp);
    
    char *tmp_dir = mkdtemp(tmp_template);
    if (!tmp_dir) {
        printf("%sfatal:%s Failed to create temporary workspace in %s.\n", GIG_CLR_ERROR, GIG_CLR_RESET, env_tmp);
        return 1;
    }

    // Download and extract in one pipeline
    snprintf(cmd, sizeof(cmd), "curl -sL -H \"User-Agent: %s\" --max-time 300 %s | tar -xzf - -C %s", USER_AGENT, url, tmp_dir);
    if (run_cmd(cmd) != 0) {
        printf("%sfatal:%s Download or extraction failed. Check your connection.\n", GIG_CLR_ERROR, GIG_CLR_RESET);
        snprintf(cmd, sizeof(cmd), "rm -rf %s", tmp_dir);
        run_cmd(cmd);
        return 1;
    }

    // 6. Structure Verification
    char inner_gigs[1024] = {0};
    snprintf(cmd, sizeof(cmd), "find %s -maxdepth 2 -type d -name \"gigs\" | tail -n 1", tmp_dir);
    if (fetch_cmd_output(cmd, inner_gigs, sizeof(inner_gigs)) != 0) {
        printf("%sfatal:%s Extracted tarball has an unexpected structure.\n", GIG_CLR_ERROR, GIG_CLR_RESET);
        snprintf(cmd, sizeof(cmd), "rm -rf %s", tmp_dir);
        run_cmd(cmd);
        return 1;
    }

    // 7. Atomic Swap Implementation
    printf("%s[SYNC]%s Applying changes...\n", GIG_CLR_CMD, GIG_CLR_RESET);
    
    char target_old[1024], target_new[1024];
    snprintf(target_old, sizeof(target_old), "%s.old", target_base);
    snprintf(target_new, sizeof(target_new), "%s/gig", tmp_dir);

    // Prepare the new 'gig' directory in tmp
    snprintf(cmd, sizeof(cmd), "mv %s %s", inner_gigs, target_new);
    run_cmd(cmd);

    // Swap: move current to .old, move new to target, delete .old
    // This ensures that we don't delete the current docs until the new ones are ready
    snprintf(cmd, sizeof(cmd), "rm -rf %s && ( [ ! -d %s ] || mv %s %s ) && mv %s %s && rm -rf %s",
             target_old, target_base, target_base, target_old, target_new, target_base, target_old);
    
    if (run_cmd(cmd) != 0) {
        printf("%sfatal:%s Failed to finalize installation. Permission issue or disk full?\n", GIG_CLR_ERROR, GIG_CLR_RESET);
        snprintf(cmd, sizeof(cmd), "rm -rf %s", tmp_dir);
        run_cmd(cmd);
        return 1;
    }

    // 8. Finalize Version
    vf = fopen(version_file, "w");
    if (vf) {
        fprintf(vf, "%s\n", tag);
        fclose(vf);
    }

    // Cleanup workspace
    snprintf(cmd, sizeof(cmd), "rm -rf %s", tmp_dir);
    run_cmd(cmd);

    printf("%s[DONE]%s Successfully synchronized to %s%s%s.\n", GIG_CLR_CMD, GIG_CLR_RESET, GIG_CLR_ARG, tag, GIG_CLR_RESET);
    return 0;
}

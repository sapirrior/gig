#!/usr/bin/env node

/**
 * man2gig - Efficiently convert man pages to GIG format.
 * Usage: node man2gig.js [section] <name>
 */

const { execSync } = require('child_process');
const fs = require('fs');
const path = require('path');

const args = process.argv.slice(2);
if (args.length === 0) {
    console.error('Usage: man2gig [section] <name>');
    process.exit(1);
}

const name = args.length === 2 ? args[1] : args[0];
const section = args.length === 2 ? args[0] : '';

try {
    // Get clean man page text
    const cmd = section ? `man ${section} ${name}` : `man ${name}`;
    const rawContent = execSync(`${cmd} | col -b`, { encoding: 'utf8', stdio: ['ignore', 'pipe', 'ignore'] });
    let lines = rawContent.split('\n').map(l => l.replace(/\t/g, '        ')); // Standard 8-space tabs

    // Filter out trailing empty lines and the footer line
    while (lines.length > 0 && !lines[lines.length - 1].trim()) lines.pop();
    if (lines.length > 0) {
        const lastLine = lines[lines.length - 1];
        if (lastLine.match(/[A-Z0-9_-]+\([1-9nl]\)\s*$/i)) {
            lines.pop();
        }
    }

    // 1. Metadata Extraction
    const firstLine = lines[0] || '';
    const metaMatch = firstLine.match(/^([A-Z0-9_-]+)\(([^)]+)\)/i);
    const pageName = metaMatch ? metaMatch[1].toLowerCase() : name;
    const pageSec = metaMatch ? metaMatch[2] : (section || '1');
    const today = new Date().toISOString().split('T')[0];

    const output = [];
    output.push(`@name    "${pageName}"`);
    output.push(`@section "${pageSec}"`);
    output.push(`@version "1.0.0"`);
    output.push(`@author  "man2gig"`);
    output.push(`@updated "${today}"`);
    output.push(`---`);
    output.push(``);

    // 2. Process Content
    for (let i = 1; i < lines.length; i++) {
        let line = lines[i];
        if (!line.trim()) {
            output.push('');
            continue;
        }

        const indent = line.search(/\S/);
        const content = line.trim();

        // Header Detection (Column 0, ALL CAPS)
        if (indent === 0 && content === content.toUpperCase() && content.length > 2) {
            output.push(`# ${content}`);
            continue;
        }

        // Flag / Definition Detection
        if (content.match(/^-(?:[a-zA-Z0-9]|-[a-z0-9-]+)/)) {
            const parts = content.split(/\s{3,}/);
            if (parts.length > 1) {
                output.push(`- ${parts[0]} : ${parts.slice(1).join(' ')}`);
            } else {
                let nextIdx = i + 1;
                while (nextIdx < lines.length && !lines[nextIdx].trim()) nextIdx++;
                
                if (nextIdx < lines.length) {
                    const nextLine = lines[nextIdx];
                    const nextIndent = nextLine.search(/\S/);
                    const nextContent = nextLine.trim();
                    
                    if (nextIndent > indent && !nextContent.startsWith('-')) {
                        output.push(`- ${content} : ${nextContent}`);
                        i = nextIdx;
                    } else {
                        output.push(`- ${content} : `);
                    }
                } else {
                    output.push(`- ${content} : `);
                }
            }
            continue;
        }

        // Bullet Detection
        if (content.match(/^[\*•o\-\+] /)) {
            output.push(`* ${content.substring(2).trim()}`);
            continue;
        }

        // Table Heuristic (Extremely conservative)
        const tableParts = content.split(/\s{5,}/).filter(p => p.length > 0);
        if (tableParts.length >= 2 && !content.startsWith('-')) {
            const hasSentencePunct = content.match(/[\.!\?]\s{2,}/);
            const isVeryLong = tableParts.some(p => p.length > 40);
            
            if (!hasSentencePunct && !isVeryLong) {
                if (tableParts.length >= 3 || (tableParts.length === 2 && tableParts[0].length < 20)) {
                    output.push(`| ${tableParts.join(' | ')}`);
                    continue;
                }
            }
        }

        // Indentation Analysis
        if (indent > 0) {
            if (indent < 7) {
                output.push(`## ${content}`);
            } else if (indent >= 14) {
                output.push(`> ${content}`);
            } else {
                const last = output.length > 0 ? output[output.length - 1].trim() : '';
                if (last === '' || last.startsWith('#') || last.startsWith('---') || last.startsWith('|') || last.startsWith('-')) {
                    output.push(`. ${content}`);
                } else {
                    output.push(`  ${content}`);
                }
            }
            continue;
        }

        output.push(`. ${content}`);
    }

    const outputPath = path.join(__dirname, `${pageName}.gg`);
    fs.writeFileSync(outputPath, output.join('\n'));
    console.log(`Success: Translated '${pageName}(${pageSec})' to '${outputPath}'`);

} catch (err) {
    console.error(`fatal: could not find or convert man page '${name}'`);
    process.exit(1);
}

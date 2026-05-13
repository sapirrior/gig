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
    const lines = rawContent.split('\n');

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
    for (let i = 1; i < lines.length - 1; i++) {
        let line = lines[i];
        if (!line.trim()) {
            output.push('');
            continue;
        }

        const indent = line.search(/\S/);
        const content = line.trim();

        if (indent === 0 && content === content.toUpperCase()) {
            output.push(`# ${content}`);
            continue;
        }

        if (content.includes('   ') && !content.startsWith('-')) {
            const parts = content.split(/\s{2,}/).filter(p => p.length > 0);
            if (parts.length > 1) {
                output.push(`| ${parts.join(' | ')}`);
                continue;
            }
        }

        if (indent === 0) {
            output.push(`. ${content}`);
        } else if (indent < 7) {
            output.push(`## ${content}`);
        } else {
            output.push(`. ${content}`);
        }
    }

    const outputPath = path.join(__dirname, `${pageName}.gg`);
    fs.writeFileSync(outputPath, output.join('\n'));
    console.log(`Success: Translated '${pageName}(${pageSec})' to '${outputPath}'`);

} catch (err) {
    console.error(`fatal: could not find or convert man page '${name}'`);
    process.exit(1);
}

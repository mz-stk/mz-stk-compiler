const express = require('express');
const { exec } = require('child_process');
const fs = require('fs').promises;
const path = require('path');
const app = express();
const port = 3000;

app.use(express.json());
app.use(express.static(path.join(__dirname, '../public')));

app.post('/run', async (req, res) => {
    const { code } = req.body;
    if (!code) {
        return res.status(400).json({ error: 'No code provided' });
    }
    
    let tempDir;
    try {
        tempDir = path.join('/tmp', `mzstk-${Date.now()}`);
        await fs.mkdir(tempDir);
        
        const baseFileName = `input-${Date.now()}`;
        const inputFileName = `${baseFileName}.mzstk`;
        const tempFileName = path.join(tempDir, inputFileName);
        await fs.writeFile(tempFileName, code);
        
        const command = `docker run --rm -v ${tempDir}:/app/temp:rw --cpus="0.5" --memory="256m" --network none mzstk-compiler bash -c "cd /app/temp && mzstk ${inputFileName} && if [ -f ${baseFileName} ]; then chmod +x ${baseFileName} && ./${baseFileName}; else echo 'Executable not found'; fi"`;
        
        exec(command, { timeout: 10000 }, async (error, stdout, stderr) => {
            await fs.rm(tempDir, { recursive: true, force: true })
                .catch(err => console.error('Failed to delete temp dir:', err));
            
            if (error) {
                return res.status(500).json({ 
                    error: stderr || error.message,
                    output: stdout
                });
            }
            
            const lines = stdout.split('\n');
            let compilationOutput = '';
            let executionOutput = '';
            let inExecutionPhase = false;
            
            for (const line of lines) {
                if (line.includes('Run with:') || line.includes('Successfully compiled')) {
                    compilationOutput += line + '\n';
                    inExecutionPhase = true;
                } else if (inExecutionPhase && !line.includes('PROGRAM') && !line.includes('Token') && !line.includes('Compiling')) {
                    executionOutput += line + '\n';
                } else if (!inExecutionPhase) {
                    compilationOutput += line + '\n';
                }
            }
            
            let output = '=== COMPILATION ===\n' + compilationOutput.trim();
            
            if (executionOutput.trim()) {
                output += '\n\n=== EXECUTION OUTPUT ===\n' + executionOutput.trim();
            }
            
            res.json({ output, error: stderr });
        });
        
    } catch (err) {
        if (tempDir) {
            await fs.rm(tempDir, { recursive: true, force: true })
                .catch(err => console.error('Failed to delete temp dir:', err));
        }
        res.status(500).json({ error: err.message });
    }
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});

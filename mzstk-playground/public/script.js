document.addEventListener('DOMContentLoaded', () => {
    const editor = document.getElementById('code-editor');
    const output = document.getElementById('output');
    const runBtn = document.getElementById('run-btn');
    const clearBtn = document.getElementById('clear-btn');

    editor.value = localStorage.getItem('lastProgram') || '# Enter your MZSTK code here\n';

    runBtn.addEventListener('click', runProgram);

    clearBtn.addEventListener('click', () => {
        output.textContent = '';
    });

    editor.addEventListener('keydown', (e) => {
        if (e.ctrlKey && e.key === 'Enter') {
            runProgram();
        }
    });

    async function runProgram() {
        output.textContent = 'Running MZSTK code...\n';
        localStorage.setItem('lastProgram', editor.value);

        try {
            const response = await fetch('/run', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ code: editor.value })
            });

            const result = await response.json();

            if (response.ok) {
                output.textContent += result.output || '';
                if (result.error) {
                    output.textContent += `\nError: ${result.error}\n`;
                }
            } else {
                output.textContent += `\nError: ${result.error}\n`;
            }
        } catch (err) {
            output.textContent += `\nError: ${err.message}\n`;
        }
    }
});

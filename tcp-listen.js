#!/usr/bin/env node
const { exec } = require('child_process');

function getListeningTCP() {
  return new Promise((resolve, reject) => {
    exec('netstat -an -p tcp', (error, stdout, stderr) => {
      if (error) {
        reject(error);
        return;
      }
      if (stderr) {
        reject(new Error(stderr));
        return;
      }

      const lines = stdout.split('\n');
      const listening = lines.filter(line => 
        line.includes('LISTEN') && 
        line.includes('tcp')
      );
      
      resolve(listening.join('\n'));
    });
  });
}

(async () => {
  try {
    const result = await getListeningTCP();
    console.log(result);
  } catch (err) {
    console.error('Error:', err.message);
    process.exit(1);
  }
})();

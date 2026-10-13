const http = require('http');
const fs = require('fs');
const path = require('path');
const { exec } = require('child_process');

// Lista de puertos preferidos (evita el 8000 que suele estar reservado por Windows NAT/Hyper-V)
const PREFERRED_PORTS = [8080, 8001, 8888, 5000, 3000, 8081, 8082];
let portIndex = 0;

const MIME_TYPES = {
  '.html': 'text/html; charset=utf-8',
  '.htm':  'text/html; charset=utf-8',
  '.js':   'application/javascript; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.bin':  'application/octet-stream',
  '.css':  'text/css; charset=utf-8',
  '.ico':  'image/x-icon',
  '.png':  'image/png'
};

function tryNextPort() {
  if (portIndex >= PREFERRED_PORTS.length) {
    console.error('No se pudo abrir ningún puerto disponible.');
    process.exit(1);
  }
  const currentPort = PREFERRED_PORTS[portIndex++];
  startServer(currentPort);
}

function startServer(targetPort) {
  const server = http.createServer((req, res) => {
    try {
      let safePath = decodeURIComponent(req.url.split('?')[0]);
      if (safePath === '/' || safePath === '') {
        safePath = '/index.html';
      }

      const filePath = path.join(__dirname, safePath);

      if (fs.existsSync(filePath) && fs.statSync(filePath).isFile()) {
        const ext = path.extname(filePath).toLowerCase();
        const contentType = MIME_TYPES[ext] || 'application/octet-stream';

        res.writeHead(200, {
          'Content-Type': contentType,
          'Access-Control-Allow-Origin': '*',
          'Cache-Control': 'no-cache'
        });

        const stream = fs.createReadStream(filePath);
        stream.pipe(res);
      } else {
        res.writeHead(404, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('404 - Archivo no encontrado');
      }
    } catch (e) {
      res.writeHead(500, { 'Content-Type': 'text/plain; charset=utf-8' });
      res.end('500 - Error interno');
    }
  });

  server.on('error', (err) => {
    if (err.code === 'EADDRINUSE' || err.code === 'EACCES') {
      tryNextPort();
    } else {
      console.error('Error del servidor:', err.message);
      tryNextPort();
    }
  });

  server.listen(targetPort, '127.0.0.1', () => {
    const url = `http://localhost:${targetPort}/`;
    console.log('=======================================================');
    console.log(`   MediaLab UniOvi - Cargador Web ESP32 (Puerto ${targetPort})`);
    console.log('=======================================================');
    console.log('');
    console.log(`[OK] Servidor activo en: ${url}`);
    console.log('[OK] Abriendo navegador predeterminado...');
    console.log('');
    console.log('>>> Por favor, NO cierres esta ventana hasta terminar <<<');
    console.log('Pulsa Ctrl+C para detener el servidor cuando acabes.');

    const startCmd = process.platform === 'win32' ? `start "" "${url}"` :
                     process.platform === 'darwin' ? `open "${url}"` : `xdg-open "${url}"`;
    exec(startCmd);
  });
}

tryNextPort();

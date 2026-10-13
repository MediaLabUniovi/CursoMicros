#!/usr/bin/env bash
cd "$(dirname "$0")"

PORT=8000
URL="http://localhost:${PORT}"

echo "======================================================="
echo "   MediaLab UniOvi - Cargador Web ESP32 (Puerto ${PORT})"
echo "======================================================="
echo ""
echo "Iniciando servidor local para Web Serial API..."
echo "Abriendo navegador en ${URL} ..."
echo "(Por favor, NO cierres esta terminal mientras cargas el firmware)"
echo ""

# Abrir el navegador en segundo plano
if command -v xdg-open > /dev/null 2>&1; then
    (sleep 1 && xdg-open "${URL}") &
elif command -v open > /dev/null 2>&1; then
    (sleep 1 && open "${URL}") &
fi

# Lanzar servidor local
if command -v python3 > /dev/null 2>&1; then
    echo "[OK] Usando python3..."
    python3 -m http.server ${PORT}
elif command -v python > /dev/null 2>&1; then
    echo "[OK] Usando python..."
    python -m http.server ${PORT}
elif command -v node > /dev/null 2>&1; then
    echo "[OK] Usando Node.js..."
    node -e "const http=require('http'),fs=require('fs'),path=require('path');http.createServer((req,res)=>{let f=path.join('.',decodeURIComponent(req.url.split('?')[0]));if(f==='.'||f==='./'||f==='.\\')f='./index.html';if(fs.existsSync(f)&&fs.statSync(f).isFile()){const m={'.html':'text/html; charset=utf-8','.js':'application/javascript; charset=utf-8','.json':'application/json','.bin':'application/octet-stream'}[path.extname(f)]||'application/octet-stream';res.writeHead(200,{'Content-Type':m});fs.createReadStream(f).pipe(res);}else{res.writeHead(404);res.end('Not found');}}).listen(8000,()=>console.log('Servidor en http://localhost:8000'));"
else
    echo "ERROR: Se requiere Python o Node.js para iniciar el servidor local."
    echo "Por favor instala Python 3 y vuelve a intentarlo."
    exit 1
fi

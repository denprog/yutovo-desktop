const CACHE_NAME = 'xcas-offline-v2.0.0';
const ASSETS_TO_CACHE = ["giac.js","giacwasm.js","giac.wasm","xcas.js","xcas.html","nws.js","numworks.js","nws.html","nws_en.html","xcasfr.html","xcasfrwasm.html","xcasen.html","codemirror.css","codemirror.js","dialog.css","dialog.js","xcasmode.js","python.js","matchbrackets.js","FileSaver.js","w3data.js","menufr.js","menuen.js","logo.png","undo.png","redo.png","config.png","longhelp.js","longhelp_en.js","giacworker.js","show-hint.js","show-hint.css","search.js","searchcursor.js","jump-to-line.js","algoseconde.html","https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.0/MathJax.js?config=TeX-AMS_CHTML"];

// Installation : on télécharge les fichiers dans le cache
self.addEventListener('install', (event) => {
  event.waitUntil(
    caches.open(CACHE_NAME).then((cache) => {
      return cache.addAll(ASSETS_TO_CACHE);
    })
  );
  self.skipWaiting(); // Force l'activation immédiate
});

// Activation : on nettoie les anciens caches ET on prend le contrôle immédiatement
self.addEventListener('activate', (event) => {
  event.waitUntil(
    caches.keys().then((cacheNames) => {
      return Promise.all(
        cacheNames.map((cache) => {
          // Si le nom du cache trouvé ne correspond pas à CACHE_NAME (ex: v2), on le supprime
          if (cache !== CACHE_NAME) {
            console.log('Suppression de l\'ancien cache :', cache);
            return caches.delete(cache);
          }
        })
      );
    }).then(() => {
      // Permet au Service Worker de contrôler la page dès l'activation
      // sans attendre que l'utilisateur recharge la page
      return self.clients.claim();
    })
  );
});

// Stratégie : Cache First (on regarde dans le cache avant le réseau)
self.addEventListener('fetch', (event) => {
  event.respondWith(
    caches.match(event.request).then((response) => {
      return response || fetch(event.request);
    })
  );
});


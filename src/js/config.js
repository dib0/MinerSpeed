Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('https://www.dib0.nl/pebble/minerspeed/config-page.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
  var config_data = JSON.parse(decodeURIComponent(e.response));

  var bitcoinaddr = config_data["bitcoinaddr"];
  localStorage.setItem("bitcoinaddr", bitcoinaddr);
  
  var msg={};
  msg.bitcoinaddr = bitcoinaddr;
  Pebble.sendAppMessage(msg);
});
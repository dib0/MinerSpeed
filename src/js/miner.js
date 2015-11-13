function trimTrailingChars(s, charToTrim)
{
  var regExp = new RegExp(charToTrim + "+$");
  var result = s.replace(regExp, "");

  return result;
}

// The miner stats from NiceHash and send this to the Pebble app
function fetchMinerStats() {
  var response;
  var req = new XMLHttpRequest();
  // build the GET request
  req.open('GET', "https://www.nicehash.com/api?method=stats.provider&location=0&addr=1DjTgkpwV11kLwv7synNtiL1zT4vpEpw2q", true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      // 200 - HTTP OK
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        var speed;
        var balance;

        if (response.result.stats[0]) {
          // data found, look for LastPrice
          speed = trimTrailingChars(response.result.stats[0].accepted_speed, '0');
          balance = response.result.stats[0].balance;
          console.log("Speed: " + speed);
          console.log("Balance: " + balance);

          var msg={};
          msg.speed = speed;
          msg.balance = balance;
          Pebble.sendAppMessage({ '0': speed, '1': balance });
        }
        else
          console.log("Response data wasn't set.");
      } else {
        console.log("Request returned error code " + req.status.toString());
      }
    }
  };
  req.send(null);
}

// Set callback for the app ready event
Pebble.addEventListener("ready", function(e) {
  console.log("connect!");

  // TODO: Bitcoin address from local storage
  // Fetch saved symbol from local storage (using
  // standard localStorage webAPI)
//  baddr = localStorage.getItem("baddr");
  fetchMinerStats();
});

// Set callback for appmessage events
Pebble.addEventListener("appmessage", function(e) {
  console.log("message");

  fetchMinerStats();
});

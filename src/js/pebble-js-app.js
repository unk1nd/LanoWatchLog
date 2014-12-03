var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  var url = "http://REMOVED BECOUSE OF REASONS!";
      console.log("URL USED IS: " + url);

  // Send request to LanoPage
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Get last action from website log
      var lastaction = json.lastaction;
      console.log("Last action on website is " + lastaction);

      // Get username that made the last action on website
      var nick = json.nick;      
      console.log("Nickname for last action is " + nick);
      
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_ACTION": lastaction,
        "KEY_NICK": nick
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Webpage logg info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending webpage logg info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the log from web
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getWeather();
  }                     
);

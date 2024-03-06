const onvif = require('onvif');

// Extract command-line arguments
const args = process.argv.slice(2); // Skip the first two elements
if (args.length != 3) {
  console.error('Usage: node getRtspUrls.js <ip_address[:port]> <username> <password>');
  process.exit(1);
}
const HOST_PORT = args[0];  // IP address and port (optional)
const USERNAME = args[1];  // Username
const PASSWORD = args[2];  // Password

// Split HOST_PORT by ':' to separate IP and port
const [HOST, PORT] = HOST_PORT.split(':');

// Default ONVIF port if not specified
const DEFAULT_PORT = 80;

// Validate input arguments
if (!HOST || !USERNAME || !PASSWORD) {
  console.error('Usage: node getRtspUrls.js <ip_address[:port]> <username> <password>');
  process.exit(1);
}

// Connect to the ONVIF camera
const camera = new onvif.Cam({
  hostname: HOST,
  username: USERNAME,
  password: PASSWORD,
  port: PORT || DEFAULT_PORT,  // Use the specified port or default if not provided
}, function(err) {
  if (err) {
    return console.error(err);
  }

  // Get all the available media profiles
  this.getProfiles((err, profiles) => {
    if (err) {
      return console.error(err);
    }

    // Fetch and output RTSP URLs for each profile
    const rtspUrls = profiles.map(profile => {
      return new Promise((resolve, reject) => {
        this.getStreamUri({protocol: 'RTSP', profileToken: profile.token}, (err, stream) => {
          if (err) {
            reject(err);
          } else {
            resolve({profileName: profile.name, rtspUri: stream.uri});
          }
        });
      });
    });

    Promise.all(rtspUrls)
      .then(urls => {
        console.log(JSON.stringify(urls, null, 2));
      })
      .catch(error => console.error(error));
  });
});

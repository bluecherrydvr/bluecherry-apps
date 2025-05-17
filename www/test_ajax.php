<?php
// Simple test page for AJAX requests
?>
<!DOCTYPE html>
<html>
<head>
    <title>AJAX Testing Tool</title>
    <script src="/template/bower_components/jquery/dist/jquery.min.js"></script>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        pre { background: #f5f5f5; padding: 10px; border: 1px solid #ddd; overflow: auto; }
        button { padding: 10px; background: #4CAF50; color: white; border: none; cursor: pointer; margin: 10px 0; }
        button:hover { background: #45a049; }
        .response { margin-top: 20px; }
        .section { margin-bottom: 30px; border-bottom: 1px solid #ddd; padding-bottom: 20px; }
    </style>
</head>
<body>
    <h1>AJAX and URL Testing Tool</h1>
    
    <div class="section">
        <h2>Test AJAX Requests</h2>
        <button id="testAjax">Test AJAX GET Request</button>
        <button id="testAjaxPost">Test AJAX POST Request</button>
        
        <div class="response">
            <h3>AJAX Response:</h3>
            <pre id="response"></pre>
        </div>
    </div>
    
    <div class="section">
        <h2>Test Direct URL Access</h2>
        <p>Click the links below to test direct URL access:</p>
        <ul>
            <li><a href="/ajax/discoverCameras.php" target="_blank">Direct: /ajax/discoverCameras.php</a></li>
            <li><a href="/discoverCameras" target="_blank">Route Format: /discoverCameras</a></li>
            <li><a href="/discover-cameras" target="_blank">Hyphenated: /discover-cameras</a></li>
        </ul>
    </div>
    
    <div class="section">
        <h2>Debug Info</h2>
        <button id="showDebug">Show Current Request Info</button>
        <div class="response">
            <h3>Debug Information:</h3>
            <pre id="debug">
Server: <?php echo $_SERVER['SERVER_NAME']; ?>

Request URI: <?php echo $_SERVER['REQUEST_URI']; ?>

PHP Version: <?php echo phpversion(); ?>

HTTP Headers:
<?php
$headers = getallheaders();
foreach ($headers as $key => $value) {
    echo $key . ': ' . $value . "\n";
}
?>
            </pre>
        </div>
    </div>
    
    <script>
        $(document).ready(function() {
            $('#testAjax').click(function() {
                $('#response').html('Loading...');
                
                $.ajax({
                    url: '/ajax/discoverCameras.php',
                    method: 'GET',
                    headers: {
                        'X-Requested-With': 'XMLHttpRequest'
                    },
                    success: function(response) {
                        try {
                            if (typeof response === 'string') {
                                $('#response').html('String response:\n' + response);
                            } else {
                                $('#response').html(JSON.stringify(response, null, 2));
                            }
                        } catch (e) {
                            $('#response').html('Error parsing response: ' + e.message + '\n\nRaw response:\n' + response);
                        }
                    },
                    error: function(xhr, status, error) {
                        $('#response').html('Error: ' + error + '\n\nStatus: ' + status + '\n\nResponse Text: ' + xhr.responseText);
                    }
                });
            });
            
            $('#testAjaxPost').click(function() {
                $('#response').html('Loading...');
                
                $.ajax({
                    url: '/ajax/discoverCameras.php',
                    method: 'POST',
                    data: { discover: 1 },
                    headers: {
                        'X-Requested-With': 'XMLHttpRequest'
                    },
                    success: function(response) {
                        try {
                            if (typeof response === 'string') {
                                $('#response').html('String response:\n' + response);
                            } else {
                                $('#response').html(JSON.stringify(response, null, 2));
                            }
                        } catch (e) {
                            $('#response').html('Error parsing response: ' + e.message + '\n\nRaw response:\n' + response);
                        }
                    },
                    error: function(xhr, status, error) {
                        $('#response').html('Error: ' + error + '\n\nStatus: ' + status + '\n\nResponse Text: ' + xhr.responseText);
                    }
                });
            });
            
            $('#showDebug').click(function() {
                $('#debug').toggle();
            });
            
            // Hide debug info by default
            $('#debug').hide();
        });
    </script>
</body>
</html> 
<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header">ONVIF Service Status</h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <div class="panel panel-default">
            <div class="panel-heading">
                <h3 class="panel-title">Connection Information</h3>
            </div>
            <div class="panel-body">
                <div class="row">
                    <div class="col-md-6">
                        <p><strong>Hostname:</strong> <?php echo $params['status']['hostname']; ?></p>
                        <p><strong>Environment:</strong> <?php echo $params['status']['environment']; ?></p>
                        <p><strong>Service URL:</strong> <?php echo $params['status']['service_url']; ?></p>
                        <p><strong>Discovery URL:</strong> <?php echo $params['status']['discovery_url']; ?></p>
                    </div>
                    <div class="col-md-6">
                        <p><strong>HTTP Status Code:</strong> 
                            <span class="<?php echo ($params['status']['http_code'] == 200) ? 'text-success' : 'text-danger'; ?>">
                                <?php echo $params['status']['http_code']; ?>
                            </span>
                        </p>
                        <p><strong>Response Time:</strong> <?php echo $params['status']['response_time']; ?></p>
                        <?php if (!empty($params['status']['curl_error'])): ?>
                            <p><strong>cURL Error:</strong> 
                                <span class="text-danger"><?php echo $params['status']['curl_error']; ?></span>
                            </p>
                        <?php endif; ?>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <div class="panel panel-default">
            <div class="panel-heading">
                <h3 class="panel-title">Response Data</h3>
            </div>
            <div class="panel-body">
                <?php if (isset($params['status']['json_valid']) && $params['status']['json_valid']): ?>
                    <?php if (isset($params['status']['data_valid']) && $params['status']['data_valid']): ?>
                        <div class="alert alert-success">
                            <p><strong>Status:</strong> The ONVIF service is working correctly.</p>
                            <p><strong>Cameras Found:</strong> <?php echo $params['status']['camera_count']; ?></p>
                        </div>
                        
                        <?php if (isset($params['status']['sample_camera']) && !empty($params['status']['sample_camera'])): ?>
                            <h4>Sample Camera Data:</h4>
                            <pre><?php print_r($params['status']['sample_camera']); ?></pre>
                        <?php endif; ?>
                    <?php else: ?>
                        <div class="alert alert-warning">
                            <p><strong>Status:</strong> The response JSON is valid, but does not have the expected structure.</p>
                        </div>
                    <?php endif; ?>
                <?php else: ?>
                    <div class="alert alert-danger">
                        <p><strong>Status:</strong> The response is not valid JSON.</p>
                        <?php if (isset($params['status']['json_error'])): ?>
                            <p><strong>Error:</strong> <?php echo $params['status']['json_error']; ?></p>
                        <?php endif; ?>
                        <?php if (isset($params['status']['response_length'])): ?>
                            <p><strong>Response Length:</strong> <?php echo $params['status']['response_length']; ?> bytes</p>
                        <?php endif; ?>
                    </div>
                <?php endif; ?>
            </div>
        </div>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <div class="panel panel-default">
            <div class="panel-heading">
                <h3 class="panel-title">Troubleshooting</h3>
            </div>
            <div class="panel-body">
                <p>If the ONVIF service is not working, check the following:</p>
                <ul>
                    <li>Ensure the ONVIF service is running at the URL shown above</li>
                    <li>Check network connectivity to the service</li>
                    <li>Verify firewall settings allow connections to the service</li>
                    <li>Look for errors in the service logs</li>
                </ul>
                
                <div class="row">
                    <div class="col-lg-12">
                        <a href="/discover-cameras" class="btn btn-primary">Return to Camera Discovery</a>
                        <a href="/onvif-status" class="btn btn-info">Refresh Status</a>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div> 
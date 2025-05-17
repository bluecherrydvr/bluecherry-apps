<?php
/**
 * Camera Discovery View
 * This view provides the interface for discovering and managing ONVIF cameras
 */
?>
<div class="row">
    <div class="col-md-12">
        <div class="panel panel-default">
            <div class="panel-heading">
                <h3 class="panel-title">Camera Discovery</h3>
            </div>
            <div class="panel-body">
                <div class="row">
                    <div class="col-md-12">
                        <button id="discover-btn" class="btn btn-primary">Discover Cameras</button>
                    </div>
                </div>
                <div class="row" style="margin-top: 20px;">
                    <div class="col-md-12">
                        <div id="discovery-results">
                            <table class="table table-striped">
                                <thead>
                                    <tr>
                                        <th>IP Address</th>
                                        <th>Manufacturer</th>
                                        <th>Model</th>
                                        <th>Status</th>
                                        <th>Actions</th>
                                    </tr>
                                </thead>
                                <tbody id="discovered-cameras">
                                </tbody>
                            </table>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<script>
$(document).ready(function() {
    $('#discover-btn').click(function() {
        var $btn = $(this);
        var $results = $('#discovered-cameras');
        
        $btn.prop('disabled', true).text('Discovering...');
        $results.empty();
        
        $.ajax({
            url: 'discover-cameras',
            method: 'POST',
            success: function(response) {
                console.log('Discovery response:', response);
                
                if (response.status === 1 && response.data) {
                    var cameras = response.data;
                    
                    cameras.forEach(function(camera) {
                        var row = $('<tr>');
                        row.append($('<td>').text(camera.ipv4));
                        row.append($('<td>').text(camera.manufacturer || 'Unknown'));
                        row.append($('<td>').text(camera.model_name || 'Unknown'));
                        row.append($('<td>').text('Available'));
                        row.append($('<td>').html(
                            '<button class="btn btn-sm btn-success add-camera" data-ip="' + camera.ipv4 + '">Add Camera</button>'
                        ));
                        $results.append(row);
                    });
                    
                    if (cameras.length === 0) {
                        $results.append('<tr><td colspan="5" class="text-center">No cameras found</td></tr>');
                    }
                } else {
                    $results.append('<tr><td colspan="5" class="text-center">Error: ' + (response.msg || 'Unknown error') + '</td></tr>');
                }
            },
            error: function(xhr, status, error) {
                console.error('Discovery request failed:', error);
                console.error('Status:', status);
                console.error('Response:', xhr.responseText);
                $results.append('<tr><td colspan="5" class="text-center">Error: ' + error + '</td></tr>');
            },
            complete: function() {
                $btn.prop('disabled', false).text('Discover Cameras');
            }
        });
    });
    
    $(document).on('click', '.add-camera', function() {
        var $btn = $(this);
        var ip = $btn.data('ip');
        
        $btn.prop('disabled', true).text('Adding...');
        
        // Add camera through the existing endpoint
        $.ajax({
            url: 'discover-cameras/add',
            method: 'POST',
            data: { ip: ip },
            success: function(response) {
                if (response.status === 1) {
                    $btn.removeClass('btn-success').addClass('btn-default')
                        .prop('disabled', true)
                        .text('Added');
                } else {
                    alert('Error: ' + (response.msg || 'Failed to add camera'));
                    $btn.prop('disabled', false).text('Add Camera');
                }
            },
            error: function(xhr, status, error) {
                console.error('Add camera failed:', error);
                alert('Error: Failed to add camera');
                $btn.prop('disabled', false).text('Add Camera');
            }
        });
    });
});
</script> 
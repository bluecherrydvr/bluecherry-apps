<div class="row">
    <div class="col-lg-12">
        <h1 class="page-header"><?php echo DISCOVER_IP_CAMERAS; ?></h1>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <div id="discover-status-msg" style="margin-bottom: 10px;">Scanning for cameras… please wait.</div>
        <div id="discover-loading" style="display:none; margin-bottom: 10px;">
            <span class="spinner-border" role="status" aria-hidden="true"></span>
        </div>
        <div class="panel panel-default">
            <div class="panel-body" style="min-height: 300px;">
                <form action="/discover-cameras/add" id="discover-cameras-list" method="post" class="form-inline">
                    <table class="table table-striped table-vert-align">
                        <thead>
                            <tr>
                                <th></th>
                                <th>IP Address</th>
                                <th>Manufacturer</th>
                                <th>Model</th>
                                <th>Authentication</th>
                                <th>Event Types</th>
                            </tr>
                        </thead>
                        <tbody>
                        <tr id="discover-cameras-list-bl-orig" style="display: none;" data-id="">
                            <td>
                                <div class="checkbox">
                                    <label>
                                        <input type="checkbox" class="discover-cameras-item-cameras" name="">
                                    </label>
                                </div>
                            </td>
                            <td>
                                <span class="discover-cameras-item-ipv4-disp"></span>
                                <input type="hidden" class="discover-cameras-item-ipv4-path" name="" value="">
                                <input type="hidden" class="discover-cameras-item-ipv4" name="" value="">
                                <input type="hidden" class="discover-cameras-item-ipv4-port" name="" value="">
                                <input type="hidden" class="discover-cameras-item-manufacturer" name="" value="">
                                <input type="hidden" class="discover-cameras-item-model-name" name="" value="">
                            </td>
                            <td>
                                <span class="discover-cameras-item-manuf-name"></span>
                            </td>
                            <td>
                                <span class="discover-cameras-item-model-name-disp"></span>
                            </td>
                            <td>
                                <div class="form-group discover-cameras-item-auth" style="display: none;">
                                    <input type="text" class="form-control discover-cameras-item-login" name="" value="" placeholder="Login">
                                    <input type="text" class="form-control discover-cameras-item-password" name="" value="" placeholder="Password">
                                </div>
                            </td>
                            <td>
                                <div class="discover-cameras-item-event-types"></div>
                            </td>
                        </tr>
                        </tbody>
                    </table>
                </form>
            </div>
        </div>
    </div>
</div>

<div class="row">
    <div class="col-lg-12">
        <form action="/discover-cameras" method="post" class="form-horizontal" style="display: inline;">
            <button type="submit" class="btn btn-success send-req-form" data-func-after="discoverCamerasFind" data-func="discoverCamerasClearList">Find cameras</button>
        </form>
        <button type="button" class="btn btn-primary send-req-form" data-form-id="discover-cameras-list" data-action="/discover-cameras/add" data-func-after="discoverCamerasAdded">Add selected cameras</button>
    </div>
</div>
<script>
document.addEventListener('DOMContentLoaded', function() {
    var shareModal = document.getElementById('share-modal');
    if (shareModal) {
        // The rest of the share-modal.js code can go here
    }
});
</script>

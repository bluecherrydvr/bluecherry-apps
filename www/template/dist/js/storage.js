$(function() {
    
    storageManageAdd();
    storageManageDelete();
    storageCheckPath();
    storageDatabaseSync();
});


function storageManageAdd() {
    $('body').on("click", "#store-add-but", function(e){
        e.preventDefault();
        $('#table-storage-orig .table-storage-tr-data').clone().appendTo('.table-storage tbody');
        $('#table-storage-orig .table-storage-tr-info').clone().appendTo('.table-storage tbody');
    });
}

function storageManageDelete() {
    
    $('body').on("click", ".store-add-del", function(e){
        e.preventDefault();
        var par = $(this).closest('tr');
        par.next().remove();
        par.remove();
    });
}

function storageCheckPath() {
    $('body').on("focusout", ".storage-path", function(e) {
        var ajax_req = new ajaxReq();
        ajax_req.manReq({
            type_req: 'GET',
            form_act : '/ajax/storagecheck.php?path='+$(this).val(),
        });
    });
}

function storageDatabaseSync() {
    $('body').on("click", "#db-sync-but", function(e) {
        e.preventDefault();
        
        // Disable button and show loading state
        var $btn = $(this);
        var originalText = $btn.html();
        $btn.prop('disabled', true).html('<i class="fa fa-spinner fa-spin fa-fw"></i> Syncing...');
        
        // Show progress message
        var $progress = $('<div class="alert alert-info" id="sync-progress"><i class="fa fa-info-circle fa-fw"></i> Database synchronization in progress...</div>');
        $('.page-header').after($progress);
        
        // Make AJAX request
        var ajax_req = new ajaxReq();
        ajax_req.manReq({
            type_req: 'POST',
            form_act: '/storage',
            form_data: {
                action: 'sync_database'
            },
            success: function(response) {
                if (response.success) {
                    $progress.removeClass('alert-info').addClass('alert-success')
                        .html('<i class="fa fa-check-circle fa-fw"></i> ' + response.message);
                    
                    // Reload page after 3 seconds to show updated storage stats
                    setTimeout(function() {
                        location.reload();
                    }, 3000);
                } else {
                    $progress.removeClass('alert-info').addClass('alert-danger')
                        .html('<i class="fa fa-exclamation-triangle fa-fw"></i> ' + response.message);
                }
            },
            error: function() {
                $progress.removeClass('alert-info').addClass('alert-danger')
                    .html('<i class="fa fa-exclamation-triangle fa-fw"></i> Database sync failed. Please try again.');
            },
            complete: function() {
                // Re-enable button
                $btn.prop('disabled', false).html(originalText);
            }
        });
    });
}

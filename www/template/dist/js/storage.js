$(function() {
    
    storageManageAdd();
    storageManageDelete();
    storageCheckPath();
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

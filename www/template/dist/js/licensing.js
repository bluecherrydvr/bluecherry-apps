$(function() {

});

function licensingAdd(form, msg) {
    if (msg.status == 'CONF') {
        $('#licensing-conf').show();
        $('#licensing-machine-id').html(msg.msg);
    } else if (msg.status == 6) {
        var ajax_req = new ajaxReq();
        ajax_req.ajaxContent('/licenses');
    }
}

function licensingBeforeAdd(form) {
    $('#licensing-conf').hide();
}

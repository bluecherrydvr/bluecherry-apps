$(function() {

});

function afterAcitvatingLicense(form, msg) {
    if (msg.status == 6) {
        var ajax_req = new ajaxReq();
        ajax_req.ajaxContent('/licenses');
        $('#general_error').html(msg.data[1]);
    }
}

function beforeAcitvatingLicense(form) {
    // Empty process
}

function licensingActivateTrial(form, msg) {
   if (msg.status == 6) {
        $('#general_error').html(msg.data[1]);
    }
}

function afterDecitvatingLicense(form, msg) {
    if (msg.status == 6) {
        var ajax_req = new ajaxReq();
        ajax_req.ajaxContent('/licenses');
        $('#general_error').html(msg.data[1]);
    }
}

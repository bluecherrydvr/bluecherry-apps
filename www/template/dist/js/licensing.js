$(function() {

});

function licensingAdd(form, msg) {
    if (msg.status == 'CONF') {
        $('#licensing-conf').show();
        $('#licensing-machine-id').html(msg.msg);
    }
}

function licensingBeforeAdd(form) {
    $('#licensing-conf').hide();
}

$(function() {
});

function ptzChangeProtocol(el) {
    var val = el.val();
    var sett_bl = $('#ptz-edit-settings');

    if (val == 'onvif') {
        sett_bl.hide();
    } else {
        sett_bl.show();
    }
}


$(function() {

});

function logDispData(form, msg) {
    var lc = new logControl(null);
    lc.dispData(msg);
    lc.scrollBottom();
}

var logControl = function (el) {
    var self = this;
    var els = {};

    self.dispData = function (data) {
        els.info_bl.find('table').html(data);
    };

    self.getData = function () {
       el.closest('form').submit();
    };

    self.scrollBottom = function () {
        var height = els.info_bl[0].scrollHeight;
        els.info_bl.scrollTop(height);
    };

    self.scrollTop = function () {
        els.info_bl.scrollTop(0);
    };
    
    var constructor = function () {
        els.info_bl = $('#log-info-bl');
    };
    constructor();
}

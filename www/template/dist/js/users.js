$(function() {
    
    usersEmailsAddDel();

    $('body').on("click", "#cameraperms-restrict-all", function(e){
        e.preventDefault();
        var cc = new cameraPrems();
        cc.restrictCams();
    });
    
    $('body').on("click", "#cameraperms-allow-all", function(e){
        e.preventDefault();
        var cc = new cameraPrems();
        cc.allowCams();
    });

    $('body').on("click", ".cameraperms-cams", function(e){
        var cc = new cameraPrems($(this));
        cc.selCam();
    });
});

var cameraPrems = function (el) {
    var self = this;
    var els = {};
    var el_one = null;

    var delClass = function () {
        els.cams.removeClass('btn-success');
        els.cams.removeClass('btn-danger');
        els.cams_icon.removeClass('fa-check');
        els.cams_icon.removeClass('fa-times');
    };

    self.allowCams = function () {
        delClass();

        els.cams.addClass('btn-success');
        els.cams_inp.prop('checked', true);
        els.cams_icon.addClass('fa-check');
        
    };

    self.restrictCams = function () {
        delClass();

        els.cams.addClass('btn-danger');
        els.cams_inp.prop('checked', false);
        els.cams_icon.addClass('fa-times');
    };

    self.selCam = function () {
        if (els.cams.hasClass('btn-success')) {
            self.restrictCams();
        } else {
            self.allowCams();
        }
    }
    
    var constructor = function (el) {
        els.cams = el || $('.cameraperms-cams');
        

        els.cams_icon = els.cams.find('i');
        els.cams_inp = els.cams.next();
    };
    constructor(el);
}

function usersAfterAdd(form, msg) {
    if (msg.status == 6) {
        var ajax_req = new ajaxReq();
        ajax_req.ajaxContent('/ajax/users.php?id='+msg.data);
    }
}

function usersEmailsAddDel() {
    $('body').on("click", ".users-add-email", function(e){
        e.preventDefault();
        var el = $('.users-add-email-inp:first').clone();
        el.find('input').val('');
        el.addClass('input-group');
        el.find('.input-group-btn').css('display', 'table-cell');
        el.appendTo('#users-add-email-inp-bl');
    });
    
    $('body').on("click", ".users-del-email", function(e){
        e.preventDefault();
        var par = $(this).closest('.users-add-email-inp');
        var count = $('.users-add-email-inp').length;
        if (count > 1) {
            par.remove();
        } 
        
        if (count == 2) {
            $('.users-add-email-inp .input-group-btn').hide();
            $('.users-add-email-inp').removeClass('input-group');
        }

    });
}


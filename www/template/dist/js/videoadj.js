$(function() {
    window.left_button_down = false;

    $('body').on('change.bfhslider', '.bfh-slider', function(e){
        if (!left_button_down) {
            videoadjSendVal($(this));
        }
    });

    $('body').on('mouseup', function(e){
        left_button_down = false;
    })
    $('body').on('mousedown.bfhslider', '.bfh-slider', function(e){
        left_button_down = true;
    });
});

function videoadjAudio(el) {

    var val_ch = 0;
    if (el.is(':checked')) val_ch = 1;

    var ajax_req = new ajaxReq();
    ajax_req.manReq({
        form_act : '/ajax/update.php',
        form_data : { id : el.data('id'), type : 'Devices', mode : 'update_control', audio_disabled : val_ch },
        type_data : 'JSON'
    });
}

function videoadjSendVal(el, indic) {
    indic =  indic || null;
    var opt = {};
    var name = el.data('name');
    opt[name] = el.closest('.form-group').find('input[name="'+name+'"]').val();

    opt.mode = el.data('mode') || 'update_control';

    var ajax_req = new ajaxReq();
    ajax_req.manReq({
        form_act : '/ajax/update.php',
        form_data : $.extend({}, { id : el.data('id'), type : 'Devices'}, opt),
        type_data : 'JSON',
        ajax_indic : indic
    });
}

function videoadjReset(el) {
    $('.bfh-slider').each(function () {
        $.valHooks.div.set($(this), 50);
        videoadjSendVal($(this), el);
    });
}

$(function() {
});

function discoverCamerasFind(form, msg) {
    var list_bl = $('#discover-cameras-list');
    var list_bl_orig = $('#discover-cameras-list-bl-orig');

    var items_html = '';
    $.each(msg.data, function(i, val) {
        var bl_clone = list_bl_orig.clone();
        bl_clone.removeAttr('id');
        bl_clone.attr('data-id', i);

        bl_clone.find('.discover-cameras-item-manuf-name').html(val.manufacturer +' '+ val.model_name);
        bl_clone.find('.discover-cameras-item-ipv4-disp').html(val.ipv4);

        var checkbox = bl_clone.find('.discover-cameras-item-cameras');
        checkbox.attr('name', 'cameras['+i+']');
        if (val.exists == 1) {
            checkbox.prop('disabled', true);
        }

        bl_clone.find('.discover-cameras-item-ipv4-path').attr('name', 'ipv4_path['+i+']').val(val.ipv4_path);
        bl_clone.find('.discover-cameras-item-ipv4').attr('name', 'ipv4['+i+']').val(val.ipv4);
        bl_clone.find('.discover-cameras-item-ipv4-port').attr('name', 'ipv4_port['+i+']').val(val.ipv4_port);
        bl_clone.find('.discover-cameras-item-manufacturer').attr('name', 'manufacturer['+i+']').val(val.manufacturer);
        bl_clone.find('.discover-cameras-item-model-name').attr('name', 'model_name['+i+']').val(val.model_name);

        bl_clone.find('.discover-cameras-item-login').attr('name', 'login['+i+']').val('');
        bl_clone.find('.discover-cameras-item-password').attr('name', 'password['+i+']').val('');

        bl_clone.show();
        list_bl_orig.after(bl_clone);
    });

}

function discoverCamerasAdded(form, msg) {
    form.find('tr').removeClass('danger');
    form.find('tr').removeClass('warning');

    $.each(msg.data.err.ip, function(i, val) {
        var parent = form.find('input[value="'+val+'"]').closest('tr').addClass('danger');
    });

    $.each(msg.data.err.passwd_ip, function(i, val) {
        var parent = form.find('input[value="'+val+'"]').closest('tr');
        parent.addClass('danger');
        parent.find('.discover-cameras-item-auth').show();
    });

    $.each(msg.data.err.rtsp_ip, function(i, val) {
        var parent = form.find('input[value="'+val+'"]').closest('tr').addClass('danger');
    });

    $.each(msg.data.err.onvif_ip, function(i, val) {
        var parent = form.find('input[value="'+val+'"]').closest('tr').addClass('warning');
    });

    $.each(msg.data.added_ip, function(i, val) {
        var parent = form.find('input[value="'+val+'"]').closest('tr');
        var checkbox = parent.find('input[type="checkbox"]');
        checkbox.prop('checked', false);
        checkbox.prop('disabled', true);
    });
}

function discoverCamerasClearList(form) {
    $('#discover-cameras-list').find('tr').not('#discover-cameras-list-bl-orig').remove();
}

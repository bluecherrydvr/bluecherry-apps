$(function() {
});

// Show loading spinner when discovery starts
$(document).on('submit', 'form[action="/discover-cameras"]', function() {
    $('#discover-loading').show();
    $('#discover-status-msg').text('Scanning for cameras… please wait.');
});

function fetchAndDisplayEventTypes(ip, port, username, password, container) {
    $.get('/events/live-scan', {
        ip: ip,
        port: port,
        username: username,
        password: password
    }, function(response) {
        if (response && response.liveTopics) {
            var eventList = $('<div class="event-types-list"></div>');
            response.liveTopics.forEach(function(type) {
                var label = type.label || type.type;
                var checkbox = $('<label style="margin-right:10px;"><input type="checkbox" class="event-type-checkbox" value="'+label+'" checked> '+label+'</label>');
                eventList.append(checkbox);
            });
            container.find('.discover-cameras-item-event-types').html(eventList);
        } else {
            container.find('.discover-cameras-item-event-types').html('<span class="text-warning">No event types found</span>');
        }
    });
}

function discoverCamerasFind(form, msg) {
    $('#discover-loading').hide();
    var list_bl = $('#discover-cameras-list');
    var list_bl_orig = $('#discover-cameras-list-bl-orig');

    // Show device count and comparison summary
    var foundCount = (msg.data && typeof msg.data.found_count !== 'undefined') ? msg.data.found_count : 0;
    var addedCount = (msg.data && msg.data.added) ? msg.data.added.length : 0;
    var removedCount = (msg.data && msg.data.removed) ? msg.data.removed.length : 0;
    var unchangedCount = (msg.data && msg.data.unchanged) ? msg.data.unchanged.length : 0;
    var summaryMsg = '';
    if (foundCount === 0) {
        summaryMsg = 'No cameras found during this ONVIF discovery.';
    } else if (addedCount === 0 && removedCount === 0) {
        summaryMsg = foundCount + ' cameras found. All unchanged since last scan.';
    } else {
        summaryMsg = foundCount + ' cameras found.';
        if (addedCount > 0) summaryMsg += ' ' + addedCount + ' new';
        if (removedCount > 0) summaryMsg += (addedCount > 0 ? ',' : '') + ' ' + removedCount + ' missing';
        summaryMsg += '.';
    }
    $('#discover-status-msg').text(summaryMsg);

    var items_html = '';
    var devices = (msg.data && msg.data.devices) ? msg.data.devices : msg.data;
    $.each(devices, function(i, val) {
        var bl_clone = list_bl_orig.clone();
        bl_clone.removeAttr('id');
        bl_clone.attr('data-id', i);

        bl_clone.find('.discover-cameras-item-manuf-name').html(val.manufacturer);
        bl_clone.find('.discover-cameras-item-model-name-disp').html(val.model_name);
        bl_clone.find('.discover-cameras-item-ipv4-disp').html(val.ipv4);

        var checkbox = bl_clone.find('.discover-cameras-item-cameras');
        checkbox.attr('name', 'cameras['+i+']');
        if (val.exists == 1) {
            checkbox.prop('disabled', true);
        }

        bl_clone.find('.discover-cameras-item-ipv4-path').attr('name', 'ipv4_path['+i+']').val(val.ipv4_path);
        bl_clone.find('.discover-cameras-item-ipv4').attr('name', 'ipv4['+i+']').val(val.ipv4);
        bl_clone.find('.discover-cameras-item-ipv4-port').attr('name', 'ipv4_port['+i+']').val(val.ipv4_port);
        // Show ONVIF port in the UI
        bl_clone.find('.discover-cameras-item-onvif-port').text(val.ipv4_port.split(':')[1] || '80');
        bl_clone.find('.discover-cameras-item-manufacturer').attr('name', 'manufacturer['+i+']').val(val.manufacturer);
        bl_clone.find('.discover-cameras-item-model-name').attr('name', 'model_name['+i+']').val(val.model_name);

        bl_clone.find('.discover-cameras-item-login').attr('name', 'login['+i+']').val('');
        bl_clone.find('.discover-cameras-item-password').attr('name', 'password['+i+']').val('');

        // Add event types container
        var eventTypesCell = $('<div class="discover-cameras-item-event-types"></div>');
        bl_clone.find('td').last().append(eventTypesCell);
        // Fetch and display event types
        fetchAndDisplayEventTypes(val.ipv4, val.ipv4_port, '', '', bl_clone);

        bl_clone.show();
        list_bl_orig.after(bl_clone);
    });
}

function discoverCamerasAdded(form, msg) {
    form.find('tr').removeClass('danger');
    form.find('tr').removeClass('warning');

    if (msg.data && msg.data.err) {
        if (msg.data.err.ip) {
            $.each(msg.data.err.ip, function(i, val) {
                var parent = form.find('input[value="'+val+'"]').closest('tr').addClass('danger');
                // Show backend error if present
                if (msg.data.err.add_error && msg.data.err.add_error[val]) {
                    parent.find('td').last().append('<div class="text-danger">'+msg.data.err.add_error[val]+'</div>');
                    console.error('Add camera error for IP '+val+':', msg.data.err.add_error[val]);
                }
            });
        }

        if (msg.data.err.passwd_ip) {
            $.each(msg.data.err.passwd_ip, function(i, val) {
                var parent = form.find('input[value="'+val+'"]').closest('tr');
                parent.addClass('danger');
                parent.find('.discover-cameras-item-auth').show();
                // Show backend error if present
                if (msg.data.err.add_error && msg.data.err.add_error[val]) {
                    parent.find('td').last().append('<div class="text-danger">'+msg.data.err.add_error[val]+'</div>');
                    console.error('Add camera error for IP '+val+':', msg.data.err.add_error[val]);
                }
            });
        }

        if (msg.data.err.rtsp_ip) {
            $.each(msg.data.err.rtsp_ip, function(i, val) {
                var parent = form.find('input[value="'+val+'"]').closest('tr').addClass('danger');
                // Show backend error if present
                if (msg.data.err.add_error && msg.data.err.add_error[val]) {
                    parent.find('td').last().append('<div class="text-danger">'+msg.data.err.add_error[val]+'</div>');
                    console.error('Add camera error for IP '+val+':', msg.data.err.add_error[val]);
                }
            });
        }

        if (msg.data.err.onvif_ip) {
            $.each(msg.data.err.onvif_ip, function(i, val) {
                var parent = form.find('input[value="'+val+'"]').closest('tr').addClass('warning');
                // Show backend error if present
                if (msg.data.err.add_error && msg.data.err.add_error[val]) {
                    parent.find('td').last().append('<div class="text-danger">'+msg.data.err.add_error[val]+'</div>');
                    console.error('Add camera error for IP '+val+':', msg.data.err.add_error[val]);
                }
            });
        }
    }

    if (msg.data && msg.data.added_ip) {
        $.each(msg.data.added_ip, function(i, val) {
            var ip = val.ip || val; // support old and new format
            var id = val.id || null;
            var parent = form.find('input[value="'+ip+'"]').closest('tr');
            var checkbox = parent.find('input[type="checkbox"]');
            checkbox.prop('checked', false);
            checkbox.prop('disabled', true);
            if (id) {
                parent.addClass('success');
                parent.find('td').last().append(
                    ' <a href="/ajax/editip.php?id='+id+'" target="_blank">Edit</a> <span class="label label-success">Added</span>'
                );
            } else {
                parent.addClass('success');
                parent.find('td').last().append(
                    ' <span class="label label-success">Added</span>'
                );
            }
        });
    }
}

function discoverCamerasClearList(form) {
    $('#discover-cameras-list').find('tbody tr').not('#discover-cameras-list-bl-orig').remove();
}

// When submitting add selected cameras, include selected event types
$(document).on('submit', 'form#discover-cameras-list', function(e) {
    $(this).find('tr').each(function() {
        var row = $(this);
        var i = row.attr('data-id');
        if (typeof i === 'undefined') return;
        var selectedTypes = [];
        row.find('.event-type-checkbox:checked').each(function() {
            selectedTypes.push($(this).val());
        });
        // Add hidden input for event types
        row.find('.discover-cameras-item-event-types input[name^="event_types"]').remove();
        row.find('.discover-cameras-item-event-types').append('<input type="hidden" name="event_types['+i+']" value="'+selectedTypes.join(',')+'">');
    });
});

// When user enters credentials, re-fetch event types
$(document).on('change', '.discover-cameras-item-login, .discover-cameras-item-password', function() {
    var row = $(this).closest('tr');
    var ip = row.find('.discover-cameras-item-ipv4').val();
    var port = row.find('.discover-cameras-item-ipv4-port').val();
    var username = row.find('.discover-cameras-item-login').val();
    var password = row.find('.discover-cameras-item-password').val();
    fetchAndDisplayEventTypes(ip, port, username, password, row);
});

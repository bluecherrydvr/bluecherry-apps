

<div class="row" style="padding:20px 0;">

    <div class="col-lg-9">

<div class="panel panel-default">
    <div class="panel-heading"><?php echo (empty($currentFormData) ? WEBHOOK_FORM_HEADING_ADD : WEBHOOK_FORM_HEADING_EDIT); ?></div>
    <div class="panel-body">
        <form method="post" id="webhook-form" action="/webhook?action=<?php echo (empty($currentFormData) ? 'create' :
            'edit&id=' . $currentFormData['id']); ?>">
            <div class="form-group">
                <label for="webhook_label"><?php echo WEBHOOK_FORM_FIELD_LABEL; ?></label>
                <input type="text" class="form-control" name="label" id="webhook_label"
                       value="<?php echo (empty($currentFormData) ? '' : $currentFormData['label']); ?>" />
            </div>
            <div class="form-group">
                <label for="webhook_url"><?php echo WEBHOOK_FORM_FIELD_URL; ?></label>
                <input type="text" class="form-control" name="url" id="webhook_url"
                       value="<?php echo (empty($currentFormData) ? '' : $currentFormData['url']); ?>" />
            </div>
            <div class="form-group">
                <label for="webhook_events"><?php echo WEBHOOK_FORM_FIELD_EVENTS; ?></label>
                <select type="text" class="form-control" name="events[]" id="webhook_events" multiple="" size="3">
                    <?php foreach($events as $event): ?>
                        <option value="<?php echo $event; ?>" <?php echo (in_array($event, $currentFormData['events']) ? 'selected="selected"' : '') ?>><?php echo constant('WEBHOOK_FORM_FIELD_EVENT_' . strtoupper($event)); ?></option>
                    <?php endforeach; ?>
                </select>
            </div>
            <div class="form-group">
                <label for="webhook_cameras"><?php echo WEBHOOK_FORM_FIELD_CAMERAS; ?></label>
                <select type="text" class="form-control" name="cameras[]" id="webhook_cameras" multiple="" size="7">
                    <?php foreach($cameras as $camera): ?>
                    <option value="<?php echo $camera['id']; ?>" <?php echo (in_array($camera['id'], $currentFormData['cameras']) ? 'selected="selected"' : '') ?>><?php echo $camera['device_name']; ?></option>
                    <?php endforeach; ?>
                </select>
            </div>
            <div class="checkbox">
                <label>
                    <input type="checkbox" name="status" value="1" <?php echo (empty($currentFormData) || !empty($currentFormData['status']) ? 'checked="checked"' : ''); ?>> <?php echo WEBHOOK_FORM_FIELD_ENABLED; ?>
                </label>
            </div>
            <button type="submit" class="btn btn-primary"><?php echo (empty($currentFormData) ? WEBHOOK_FORM_BUTTON_CREATE : WEBHOOK_FORM_BUTTON_UPDATE); ?></button>
        </form>
    </div>
</div>

    </div>

    <div class="col-lg-3">

<?php if (!empty($webhooks)): ?>
<div class="panel panel-default">
    <div class="panel-heading"><?php echo WEBHOOK_LISTING_HEADER; ?></div>
    <div class="panel-body">
        <table class="table table-bordered">
            <thead>
                <tr>
                    <th><?php echo WEBHOOK_LISTING_TABLE_HEADER_LABEL; ?></th>
                    <th colspan="2" class="text-center"><?php echo WEBHOOK_LISTING_TABLE_HEADER_ACTIONS; ?></th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($webhooks as $item): ?>
                <tr>
                    <td><?php echo ($item['status'] == 1 ? '✔️ ' : '❌ '), $item['label'] ?><br /><?php echo WEBHOOK_LISTING_TABLE_LAST_UPDATE , ' ', $item['last_update']; ?></td>
                    <td class="text-center" style="width:100px;"><a href="/webhook?action=edit&id=<?php echo $item['id'] ?>" class="btn btn-primary"><?php echo WEBHOOK_LISTING_TABLE_BUTTON_EDIT; ?></a></td>
                    <td class="text-center" style="width:100px;"><button value="/webhook?action=delete&id=<?php echo $item['id'] ?>" class="btn btn-danger" id="btn-webhook-delete"><?php echo WEBHOOK_LISTING_TABLE_BUTTON_DELETE; ?></button></td>
                </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    </div>
</div>
<?php endif; ?>

        <?php if (!empty($currentFormData)): ?>
        <a href="/webhook" class="btn btn-success"><?php echo WEBHOOK_LISTING_TABLE_BUTTON_CREATE_NEW; ?></a>
        <?php endif; ?>

    </div>
</div>

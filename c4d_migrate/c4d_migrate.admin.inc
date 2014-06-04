<?php
/**
 * @file
 * Administration screens and forms for c4d_export.
 */

/**
 * Form callback for admin/content/migrate/import.
 */
function c4d_migrate_admin_overview_form() {
  $form = array();

  $form['database'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Database settings'),
    '#collapsible' => FALSE,
  );

  $form['database'][C4D_MIGRATE_DB_HOST] = array(
    '#type'          => 'textfield',
    '#title'         => t('Database host'),
    '#default_value' => variable_get(C4D_MIGRATE_DB_HOST, '127.0.0.1'),
    '#size'          => 60,
    '#maxlength'     => 128,
    '#required'      => TRUE,
  );

  $form['database'][C4D_MIGRATE_DB_USER] = array(
    '#type'          => 'textfield',
    '#title'         => t('Database username'),
    '#default_value' => variable_get(C4D_MIGRATE_DB_USER, ''),
    '#size'          => 60,
    '#maxlength'     => 128,
    '#required'      => FALSE,
  );

  $form['database'][C4D_MIGRATE_DB_PASS] = array(
    '#type'        => 'password',
    '#title'       => t('Database password'),
    '#description' => t('No password has been set yet. Leave empty for unrestricted access.'),
    '#size'        => 60,
    '#maxlength'   => 128,
    '#required'    => FALSE,
  );
  if (variable_get(C4D_MIGRATE_DB_PASS, NULL)) {
    $form['database'][C4D_MIGRATE_DB_PASS]['#description'] = t(
      'A password has been entered yet (not displayed), enter it again if you want to save changes to any of the settings on this page.'
    );
  }

  $form['database']['c4d_migrate_pw_reset'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Remove password (make empty)'),

  );

  $form['database'][C4D_MIGRATE_DB_NAME] = array(
    '#type'          => 'textfield',
    '#title'         => t('Database name'),
    '#default_value' => variable_get(C4D_MIGRATE_DB_NAME, ''),
    '#size'          => 60,
    '#maxlength'     => 128,
    '#required'      => TRUE,
  );

  $form['files'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('File settings'),
    '#collapsible' => FALSE,
  );

  $form['files'][C4D_MIGRATE_FILES_ROOT] = array(
    '#type'          => 'textfield',
    '#title'         => t('capacity4dev Root location'),
    '#description'   => t('Provide the full path to the root of the old capacity4dev platform.'),
    '#default_value' => variable_get(C4D_MIGRATE_FILES_ROOT, '/'),
    '#size'          => 60,
    '#maxlength'     => 128,
    '#required'      => TRUE,
  );

  return system_settings_form($form);
}

/**
 * Validate handler for c4d_migrate_admin_overview_form.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 */
function c4d_migrate_admin_overview_form_validate($form, &$form_state) {
  if (empty($form_state['values'][C4D_MIGRATE_DB_PASS])
    && $form_state['values'][C4D_MIGRATE_DB_PASS] !== variable_get(C4D_MIGRATE_DB_PASS, NULL)
    && !$form_state['values']['c4d_migrate_pw_reset']
  ) {
    $form_state['values'][C4D_MIGRATE_DB_PASS] = variable_get(C4D_MIGRATE_DB_PASS, NULL);
  }
}

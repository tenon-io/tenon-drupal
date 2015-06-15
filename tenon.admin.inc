<?php

/**
 * Admin settings form.
 */
function tenon_admin_settings_form($form, &$form_state) {
  $form['api_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tenon.io API settings'),
  );
  $form['api_settings']['tenon_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('API key'),
    '#required' => TRUE,
    '#description' => t('You can find your key in your dashboard in the "API Key" section.'),
    '#default_value' => variable_get('tenon_api_key', ''),
  );
  return system_settings_form($form);
}

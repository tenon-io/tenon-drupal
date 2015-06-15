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
    '#default_value' => variable_get('tenon_api_key', NULL),
  );
  return system_settings_form($form);
}

/**
 * Trigger a page report.
 */
function tenon_trigger_page_report() {
  $output = '';

  // First of all, check if the user provided an API key.
  $api_key = variable_get('tenon_api_key', NULL);
  if (empty($api_key)) {
    // If not, be kind and invite him to do so.
    drupal_set_message(t('Wait! We are not able to generate a report while you do not provide us an API key! !api_settings_page', array('!api_settings_page' => l('Fix it!', 'admin/config/user-interface/tenon'))), 'warning');
    watchdog('tenon_api', 'Page report: API request attempt without credentials.');
    drupal_access_denied();
    return;
  }
  // Otherwise, let's eat the API.
  $url = TENON_IO_API_URL . '';
  $tested_url = 'http://cdiscount.com';
  $options = array(
    'headers' => array(
      'Content-Type' => 'application/x-www-form-urlencoded',
    ),
    'method' => 'POST',
    'data' => 'key=' . $api_key . '&url=' . urlencode($tested_url),
  );
  $request = drupal_http_request($url, $options);

  // Analyze the request response.
  switch ($request->code) {
    case 200:
      // If we have a valid answer, decode the data.
      $data = drupal_json_decode($request->data);

      // Fetch the results and format the response for the end user.
      $all_count = $data['resultSummary']['density']['allDensity'];
      $errors_count = $data['resultSummary']['density']['errorDensity'];
      $warnings_count = $data['resultSummary']['density']['warningDensity'];

      // Celebrate if there are no error.
      if ($all_count == 0) {
        $output .= '<p>' . t('Congratulations, you do not have any warning or error on your page.') . '</p>';
      }
      else {
        // Check if all the issues are errors.
        if ($errors_count == $all_count) {
          $output .= '<p>' . format_plural($errors_count, '1 error found for !tested_url', '!count_errors errors found for !tested_url', array('!count_errors' => $errors_count, '!tested_url' => $tested_url)) . '</p>';
        }
        // Then check if all the issues are warning.
        else if ($warnings_count == $all_count) {
          $output .= '<p>' . format_plural($errors_count, '1 warning found for !tested_url', '!count_warnings warnings found for !tested_url', array('!count_warnings' => $warnings_count, '!tested_url' => $tested_url)) . '</p>';
        }
        // If not, we need to display a mix of both.
        else {
          $output .= '<p>' . t('!count issues found for !tested_url.', array('!count' => $all_count, '!tested_url' => $tested_url));
          $output .= format_plural($warnings_count, '1 warning', '!count_warnings warnings', array('!count_warnings' => $warnings_count));
          $output .= t(' and ') . format_plural($errors_count, '1 error', '!count_errors errors', array('!count_errors' => $errors_count)) . '.</p>';
        }
      }
      $output .= '<p>' . l(t('View full results on Tenon.io.'), '#TODO') . '</p>';
      break;
    default:
      break;
  }
  return $output;
}


/**
 * Trigger a spider report.
 */
function tenon_trigger_spider_report() {

}

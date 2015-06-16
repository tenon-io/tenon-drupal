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
  $form['api_advanced_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tenon.io advanced API settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['api_advanced_settings']['tenon_app_id'] = array(
    '#type' => 'textfield',
    '#title' => t('App ID'),
    '#description' => t('You can find your app ID in your dashboard after creating it in the "Applications" section.'),
    '#default_value' => variable_get('tenon_app_id', NULL),
  );
  $form['api_advanced_settings']['tenon_certainty'] = array(
    '#type' => 'radios',
    '#title' => t('Certainty'),
    '#description' => t('The certainty parameter allows you to filter out these "uncertain" results by choosing a minimum acceptable certainty score.'),
    '#options' => drupal_map_assoc(array(0, 20, 40, 60, 80, 100)),
    '#default_value' => variable_get('tenon_certainty', 0),
  );
  $form['api_advanced_settings']['tenon_importance'] = array(
    '#type' => 'radios',
    '#title' => t('Importance'),
    '#description' => t('This parameter is used when calculating final issue priority in the results. The importance parameter is only really relevant or useful when compiling a report set consisting of multiple documents.'),
    '#options' => array(
      TENON_IMPORTANCE_NONE => t('None'),
      TENON_IMPORTANCE_LOW => t('Low'),
      TENON_IMPORTANCE_MEDIUM => t('Medium'),
      TENON_IMPORTANCE_HIGH => t('High'),
    ),
    '#default_value' => variable_get('tenon_importance', TENON_IMPORTANCE_NONE),
  );
  $form['api_advanced_settings']['tenon_level'] = array(
    '#type' => 'radios',
    '#title' => t('Level'),
    '#description' => t('The level parameter indicates the "lowest" WCAG level to test against.'),
    '#options' => drupal_map_assoc(array('AAA', 'AA', 'A')),
    '#default_value' => variable_get('tenon_level', 'AAA'),
  );
  $form['api_advanced_settings']['tenon_priority'] = array(
    '#type' => 'radios',
    '#title' => t('Priority'),
    '#description' => t(''),
    '#options' => drupal_map_assoc(array(0, 20, 40, 60, 80, 100)),
    '#default_value' => variable_get('tenon_priority', 0),
  );
  $form['api_advanced_settings']['tenon_ref_information'] = array(
    '#type' => 'checkbox',
    '#title' => t('Reference information'),
    '#description' => t('Generate a link with each issue which includes reference information for the violated Best Practice.'),
    '#default_value' => variable_get('tenon_ref_information', TRUE),
  );
  $form['api_advanced_settings']['tenon_store_information'] = array(
    '#type' => 'radios',
    '#title' => t('Store information'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#description' => t('Store your test results on Tenon to come back later and view your results in our Web-based system.'),
    '#default_value' => variable_get('tenon_store_information', 0),
  );
  $form['api_advanced_settings']['tenon_project_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Project ID'),
    '#maxlength' => 255,
    '#size' => 20,
    '#description' => t('String of text you can supply to identify the tested document as part of a specific system. This is especially useful if you are developing or testing multiple projects.'),
    '#default_value' => variable_get('tenon_project_id', ''),
  );
  $form['api_advanced_settings']['tenon_ua_string'] = array(
    '#type' => 'textfield',
    '#title' => t('User-Agent string'),
    '#maxlength' => 255,
    '#size' => 50,
    '#description' => t('Arbitrary string of text which will be sent as the User-Agent string when we request the URL you supply. This is particularly useful if your site does any user-agent sniffing.'),
    '#default_value' => variable_get('tenon_project_id', ''),
  );
  // @TODO: apply digit only validation.
  $form['api_advanced_settings']['tenon_viewport_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Viewport width'),
    '#maxlength' => 4,
    '#size' => 5,
    '#description' => t('Width, in pixels, of the viewport for our headless browser. The value must between 0 and 9999.'),
    '#default_value' => variable_get('tenon_viewport_width', TENON_VIEWPORT_DEFAULT_WIDTH),
  );
  // @TODO: apply digit only validation.
  $form['api_advanced_settings']['tenon_viewport_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Viewport height'),
    '#maxlength' => 4,
    '#size' => 5,
    '#description' => t('Height, in pixels, of the viewport for our headless browser. The value must between 0 and 9999.'),
    '#default_value' => variable_get('tenon_viewport_height', TENON_VIEWPORT_DEFAULT_HEIGHT),
  );

  return system_settings_form($form);
}

/**
 * Trigger a page report.
 */
function tenon_trigger_page_report() {
  $tested_url = url(current_path(), array('absolute' => TRUE));

  // First of all, check if the user provided an API key.
  $api_key = variable_get('tenon_api_key', NULL);
  if (empty($api_key)) {
    // If not, be kind and invite him to do so.
    drupal_set_message(t('Wait! We are not able to generate a report while you do not provide us an API key! !api_settings_page', array('!api_settings_page' => l('Fix it!', 'admin/config/user-interface/tenon'))), 'warning');
    watchdog('tenon_api', 'Page report: API request attempt without credentials.');
    drupal_access_denied();
    return;
  }

  // Build the query to the API.
  $i = 0;
  $data = '';
  $api_parameters = array(
    'key'             => $api_key,
    'url'             => $tested_url,
    'appId'           => variable_get('tenon_app_id', ''), // Remove if empty?
    'projectID'       => variable_get('tenon_project_id', ''), // Remove if empty?
    'certainty'       => variable_get('tenon_certainty', 0),
    'importance'      => variable_get('tenon_importance', 0),
    'level'           => variable_get('tenon_level', 'AAA'),
    'priority'        => variable_get('tenon_priority', 0),
    'ref'             => variable_get('tenon_ref', TRUE),
    'store'           => variable_get('tenon_store', 0),
    'uaString'        => variable_get('tenon_ua_string', ''),
    'viewPortHeight'  => variable_get('tenon_viewport_height', TENON_VIEWPORT_DEFAULT_HEIGHT),
    'viewPortWidth'   => variable_get('tenon_viewport_width', TENON_VIEWPORT_DEFAULT_WIDTH),
  );
  foreach ($api_parameters as $parameter_name => $parameter_value) {
    $separator = '';
    if ($i++ != 0) {
      $separator = '&';
    }
    $data .= $separator . $parameter_name . '=' . urlencode($parameter_value);
  }

  // Query the API.
  $options = array(
    'headers' => array(
      'Content-Type' => 'application/x-www-form-urlencoded',
    ),
    'method' => 'POST',
    'data' => $data,
  );
  $output = tenon_query_api($options, $tested_url);
  return $output;
}

/**
 * Query the API.
 *
 * @param $options
 *   Array of data describing the API to query. Those data are forwarded to
 *   drupal_http_request().
 * @param $tested_url
 *   String of the URL to test the API against.
 *
 * @return
 *   Returns the output of the query.
 */
function tenon_query_api($options, $tested_url) {
  $request = drupal_http_request(TENON_IO_API_URL, $options);

  // Check if the request went through without any trouble.
  if ($request->code == 200) {
    // If we have a valid answer, decode the data.
    $data = drupal_json_decode($request->data);
    return tenon_query_display_page_results($tested_url, $data);
  }
  else {
    // Otherwise, explain the issue to the user and log it.
    return tenon_query_handle_errors($request);
  }
}

/**
 * Analyzes the code returned by a query to the API.
 *
 * @param $request
 *   Object coming from the drupal_http_request() query.
 *
 * @return
 *   Return output detailing the cause of the error.
 */
function tenon_query_handle_errors($request) {
  // Explain the issue to the user and log it.
  // If we don't have data in the request, it means that the server did not
  // answer and didn't provide its own metadata about the error.
  if (!isset($request->data)) {
    // In this case, log a generic error message.
    $output = '<p>' . t('An error occured, we deeply apologize.') . '</p>';
    drupal_set_message(t('The connection to the ressource timed out.'), 'error');
    watchdog('tenon_api_error', 'Error !code: !error.', array(
      '!code' => $request->code,
      '!message' => $request->error
    ));
    return $output;
  }
  else {
    // Otherwise, decode the response from the API and analyze the error.
    $message_type = 'error';
    $message_variables = array();
    $data = drupal_json_decode($request->data);

    // Analyze the status code returned by the query.
    switch ($data['status']) {
      // Sample data to test?
      case 500:
        $message = 'An internal error occured, please try to submit your request again.';
        break;
      case 522:
        // Sample data to test?
        $message = 'The connection to the resource timed out.';
        break;
      case 401:
        // Identity the cause of the unauthorized access.
        if ($data['code'] == 'api_credentials_invalid') {
          $message_type = 'warning';
          $message = 'Please verify your API credentials. !settings_link.';
          $message_variables = array('!settings_link' => l(t('Verify the settings page'), 'admin/config/user-interface/tenon'));
        }
        else if ($data['code'] == 'monthly_limit_reached') {
          // Sample data to test?
          $message = 'It looks like you have reached your monthly limit.';
        }
        else {
          $message = 'The connection to the API is unauthorized.';
        }
        break;
      case 400:
        $message = 'The request is malformed.';

        // Identity the cause of the unauthorized access.
        switch ($data['code']) {
          case 'invalid_param':
            $message_type = 'warning';
            $message = '!invalid_param_message <br /> Please verify your settings. !settings_link.';
            $message_variables = array(
              '!settings_link' => l(t('Access the settings page'), 'admin/config/user-interface/tenon'),
              '!invalid_param_message' => $data['message']
            );
            break;
          case 'blank_url_or_src':
            $message = '!invalid_param_message <br /> Please verify the tested URL.';
            $message_variables = array(
              '!invalid_param_message' => $data['message']
            );
            break;
          case 'bad_src':
            $message = '!invalid_param_message <br /> Please verify the tested SRC.';
            $message_variables = array(
              '!invalid_param_message' => $data['message']
            );
            break;
          case 'abuse_detected':
            $message = 'An abuse has been detected.';
            break;
          case 'required_param_missing':
            $message = '!invalid_param_message';
            $message_variables = array(
              '!invalid_param_message' => $data['message']
            );
            break;
          case 'doc_source_too_big':
            // Sample data to test?
            $message = 'The content submitted is too big to be analyzed.';
            break;
          case 'improper_content_type':
            // Sample data to test?
            $message = 'The submitted resource does not appear to be a valid content type.';
            break;
          case 'url_request_failed':
            // Sample data to test?
            $message = 'The attempt to test the requested URL failed.';
            break;
        }
        break;
      default:
        $message = 'An error occured.';
        break;
    }
    // Display a message to the user and log the error.
    drupal_set_message(t($message, $message_variables), $message_type);
    watchdog('tenon_api_error', 'Error !code: !error.', array(
      '!code' => $data['status'],
      '!message' => $data['message']
    ));

    // @TODO: improve this user message.
    $output = '<p>' . t('An error occured, we deeply apologize.') . '</p>';

    // If we have a link to provide with more explanations of the cause of
    // the error, provide it to the user.
    if (isset($data->moreInfo)) {
      $output .= '<p>' . l(t('View the error details'), $data->moreInfo) . '</p>';
      return $output;
    }
    return $output;
  }
}

/**
 * Format the page test results.
 *
 * @param $tested_url
 *   URL of the tested site.
 * @param $data
 *   Array of data returned by the API.
 *
 * @return
 *   Output of the formatted results.
 */
function tenon_query_display_page_results($tested_url, $data) {
  // Fetch the results and format the response for the end user.
  $all_count = $data['resultSummary']['density']['allDensity'];
  $errors_count = $data['resultSummary']['density']['errorDensity'];
  $warnings_count = $data['resultSummary']['density']['warningDensity'];

  // Celebrate if there are no error.
  $output = '';
  if ($all_count == 0) {
    $output .= '<p>' . t('Congratulations, you do not have any warning or error on your page.') . '</p>';
  }
  else {
    // Check if all the issues are errors.
    if ($errors_count == $all_count) {
      $output .= '<p>' . format_plural($errors_count, '1 error found for !tested_url', '!count_errors errors found for !tested_url', array(
          '!count_errors' => $errors_count,
          '!tested_url' => $tested_url
        )) . '</p>';
    }
    // Then check if all the issues are warning.
    else {
      if ($warnings_count == $all_count) {
        $output .= '<p>' . format_plural($errors_count, '1 warning found for !tested_url', '!count_warnings warnings found for !tested_url', array(
            '!count_warnings' => $warnings_count,
            '!tested_url' => $tested_url
          )) . '</p>';
      }
      // If not, we need to display a mix of both.
      else {
        $output .= '<p>' . t('!count issues found for !tested_url.', array(
            '!count' => $all_count,
            '!tested_url' => $tested_url
          ));
        $output .= format_plural($warnings_count, '1 warning', '!count_warnings warnings', array('!count_warnings' => $warnings_count));
        $output .= t(' and ') . format_plural($errors_count, '1 error', '!count_errors errors', array('!count_errors' => $errors_count)) . '.</p>';
      }
    }
  }
  $output .= '<p>' . l(t('View full results on Tenon.io.'), '#TODO') . '</p>';
  return $output;
}

/**
 * Trigger a spider report.
 */
function tenon_trigger_spider_report() {

}

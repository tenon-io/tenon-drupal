<?php

/**
 * Admin settings form.
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function tenon_admin_settings_form($form, &$form_state) {
  $form['api_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tenon.io API settings'),
  );
  $form['api_settings']['tenon_api_endpoint_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Page test endpoint URL'),
    '#required' => TRUE,
    '#description' => t('URL of the endpoint for a single page test. Default url is !default_url.', array('!default_url' => TENON_IO_API_URL)),
    '#default_value' => variable_get('tenon_api_endpoint_page', TENON_IO_API_URL),
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
    '#type' => 'hidden',
    '#title' => t('Store information'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#description' => t('Store your test results on Tenon to come back later and view your results in our Web-based system.'),
    '#default_value' => variable_get('tenon_store_information', 1),
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
  $form['api_advanced_settings']['tenon_viewport_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Viewport width'),
    '#maxlength' => 4,
    '#size' => 5,
    '#required' => TRUE,
    '#description' => t('Width, in pixels, of the viewport for our headless browser. The value must between 0 and 9999.'),
    '#default_value' => variable_get('tenon_viewport_width', TENON_VIEWPORT_DEFAULT_WIDTH),
    '#element_validate' => array('tenon_validate_numeric_value'),
  );
  $form['api_advanced_settings']['tenon_viewport_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Viewport height'),
    '#maxlength' => 4,
    '#size' => 5,
    '#required' => TRUE,
    '#description' => t('Height, in pixels, of the viewport for our headless browser. The value must between 0 and 9999.'),
    '#default_value' => variable_get('tenon_viewport_height', TENON_VIEWPORT_DEFAULT_HEIGHT),
    '#element_validate' => array('tenon_validate_numeric_value'),
  );

  return system_settings_form($form);
}

/**
 * Validation callback to check that a value is a positive numeric value.
 * @param $element
 */
function tenon_validate_numeric_value($element) {
  $value = $element['#value'];
  $regexp = '@([^0-9]{1,4})@';
  if ($value != preg_replace($regexp, '', $value)) {
    form_error($element, t('The %name must be between 0 and 9999.', array('%name' => $element['#title'])));
  }
}

/**
 * Trigger a page report.
 * @return string|void
 */
function tenon_trigger_page_report() {
  $tested_url = url(current_path(), array('absolute' => TRUE));
  if (!empty($_GET['url'])) {
    $tested_url = $_GET['url'];
  }
  // First of all, check if the user provided an API key.
  $api_key = variable_get('tenon_api_key', NULL);
  if (empty($api_key)) {
    // If not, be kind and invite him to do so.
    drupal_set_message(t('Wait! We are not able to generate a report while you do not provide us an API key! !api_settings_page', array('!api_settings_page' => l(t('Fix it!'), 'admin/config/user-interface/tenon'))), 'warning');
    watchdog('tenon_api', 'Page report: API request attempt without credentials.');
    drupal_access_denied();
    return;
  }

  // Build the query to the API.
  list($endpoint_url, $options) = tenon_query_build($tested_url, $api_key);

  $request = drupal_http_request($endpoint_url, $options);

  // Check if the request went through without any trouble.
  if ($request->code == 200) {
    // If we have a valid answer, decode the data.
    $data = drupal_json_decode($request->data);
    return tenon_query_display_page_results_full($tested_url, $data);
  }
  else {
    // Otherwise, explain the issue to the user and log it.
    return tenon_query_handle_errors_html($request);
  }
}

/**
 * Format error messages for HTML.
 *
 * @param $request
 *   Response object from Tenon.io API.
 *
 * @return
 *   String to display to the user.
 */
function tenon_query_handle_errors_html($request) {
// Explain the issue to the user and log it.
  // If we don't have data in the request, it means that the server did not
  // answer and didn't provide its own metadata about the error.
  if (!isset($request->data)) {
    // In this case, log a generic error message.
    $output = '<p>' . t('An error occurred, We deeply apologize.') . '</p>';
    drupal_set_message(t('The connection to the resource timed out.'), 'error');
    watchdog('tenon_api_error', 'Error !code: !error.', array(
      '!code' => $request->code,
      '!error' => $request->error
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
        $message = 'An internal error occurred, Please try to submit your request again.';
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
        else {
          if ($data['code'] == 'monthly_limit_reached') {
            // Sample data to test?
            $message = 'It looks like you have reached your monthly limit.';
          }
          else {
            $message = 'The connection to the API is unauthorized.';
          }
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
    $output = '<p>' . t('An error occurred, we deeply apologize.') . '</p>';

    // If we have a link to provide with more explanations of the cause of
    // the error, provide it to the user.
    if (isset($data['moreInfo'])) {
      $output .= '<p>' . l(t('View the error details'), $data['moreInfo']) . '</p>';
      return $output;
    }
    return $output;
  }
}

/**
 * Format error messages for AJAX.
 *
 * @param $request
 *   Response object from Tenon.io API.
 *
 * @return
 *   Returns the data array to send back to the AJAX request.
 *   Expected keys of the array are:
 *   - content: String of the error message.
 *   - link: An html link to the details of the error.
 *   - message_type:  A string of the message type.
 */
function tenon_query_handle_errors_ajax($request) {
  // Explain the issue to the user and log it.
  // If we don't have data in the request, it means that the server did not
  // answer and didn't provide its own metadata about the error.
  if (!isset($request->data)) {
    // In this case, log a generic error message.
    $output = '<p>' . t('An error occured, we deeply apologize, the request timed out.') . '</p>';
    watchdog('tenon_api_error', 'Error !code: !error.', array(
      '!code' => $request->code,
      '!error' => $request->error
    ));
    return array(
      'content' => $output,
      'link' => '',
      'message_type' => '',
    );
  }
  else {
    // Otherwise, decode the response from the API and analyze the error.
    $message_type = 'error';
    $message_variables = array();
    $link = '';
    $data = drupal_json_decode($request->data);

    // Analyze the status code returned by the query.
    switch ($data['status']) {
      // Sample data to test?
      case 500:
        $message = 'An internal error occurred, please try to submit your request again.';
        break;
      case 522:
        // Sample data to test?
        $message = 'The connection to the resource timed out.';
        break;
      case 401:
        // Identity the cause of the unauthorized access.
        if ($data['code'] == 'api_credentials_invalid') {
          $message_type = 'warning';
          $message = 'Please verify your API credentials.';
          $link = l(t('Verify the settings page'), 'admin/config/user-interface/tenon');
        }
        else {
          if ($data['code'] == 'monthly_limit_reached') {
            // Sample data to test?
            $message = 'It looks like you have reached your monthly limit.';
          }
          else {
            $message = 'The connection to the API is unauthorized.';
          }
        }
        break;
      case 400:
        $message = 'The request is malformed.';

        // Identity the cause of the unauthorized access.
        switch ($data['code']) {
          case 'invalid_param':
            $message_type = 'warning';
            $message = '!invalid_param_message <br /> Please verify your settings.';
            $link = l(t('Access the settings page'), 'admin/config/user-interface/tenon');
            $message_variables = array(
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
    // Log the error.
    watchdog('tenon_api_error', 'Error !code: !error.', array(
      '!code' => $data['status'],
      '!message' => $data['message']
    ));

    // @TODO: improve this user message.
    // We need to call format_string() to keep the translation flexibility.
    // We have a piece of the message that comes from the API so we need to have
    // a variable for that.
    $output = '<p>' . t('An error occured, we deeply apologize. <br /> !message', array('!message' => format_string($message, $message_variables))) . '</p>';

    // If we have a link to provide with more explanations of the cause of
    // the error, provide it to the user.
    if (isset($data['moreInfo'])) {
      $link = l(t('View the error details'), $data['moreInfo']);
    }

    // Format the data for the API.
    return array(
      'content' => $output,
      'link' => $link,
      'message_type' => $message_type,
    );
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
function tenon_query_display_page_results_full($tested_url, $data) {
  // Extract the results and format the response for the end user.
  $issue_count = $data['resultSummary']['issues']['totalIssues'];
  $error_count = $data['resultSummary']['issues']['totalErrors'];
  $warning_count = $data['resultSummary']['issues']['totalWarnings'];
  $report_url = 'https://tenon.io/history.php?responseID=' . $data['request']['responseID'];

  // Store the number of issues for a given URL.
  $url_hash = md5($tested_url);
  cache_set('tenon_' . $url_hash, $issue_count);

  $variables = array(
    'report_url' => l(t('View full results on Tenon.io'), $report_url),
    'error_count' => $error_count,
    'warning_count' => $warning_count,
    'issues_count' => $issue_count,
    'a_level_count' => $data['resultSummary']['issuesByLevel']['A']['count'],
    'a_level_percentage' => $data['resultSummary']['issuesByLevel']['A']['pct'],
    'aa_level_count' => $data['resultSummary']['issuesByLevel']['AA']['count'],
    'aa_level_percentage' => $data['resultSummary']['issuesByLevel']['AA']['pct'],
    'aaa_level_count' => $data['resultSummary']['issuesByLevel']['AAA']['count'],
    'aaa_level_percentage' => $data['resultSummary']['issuesByLevel']['AAA']['pct'],
    'tested_url' => $tested_url,
  );
  return theme('tenon_results_report_full', $variables);
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
function tenon_query_display_page_results_summary($tested_url, $data) {
  // Extract the results and format the response for the end user.
  $issue_count = $data['resultSummary']['issues']['totalIssues'];
  $error_count = $data['resultSummary']['issues']['totalErrors'];
  $warning_count = $data['resultSummary']['issues']['totalWarnings'];

  // Store the number of issues for the tested URL.
  $url_hash = md5($tested_url);
  cache_set('tenon_' . $url_hash, $issue_count);

  $variables = array(
    'error_count' => $error_count,
    'warning_count' => $warning_count,
    'issues_count' => $issue_count,
  );
  return theme('tenon_results_report_summary', $variables);
}

/**
 * Trigger a spider report.
 */
function tenon_trigger_spider_report() {
  // @TODO once the API endpoint exists.
}

/**
 * Format the page test's response of the API in JSON.
 *
 * Returns the data array to send back to the AJAX request.
 * Expected keys of the array are:
 *  - content: String of the error message.
 *  - link: An html link to the details of the error.
 *  - message_type:  A string of the message type.
 */
function tenon_trigger_ajax_page_report() {
  // First of all, check if the user provided an API key.
  $api_key = variable_get('tenon_api_key', NULL);
  if (empty($api_key)) {
    // If not, be kind and invite him to do so.
    $content = array(
      'content' => '<p>' . t('Wait! We are not able to generate a report while you do not provide us an API key!') . '</p>',
      'link' => l(t('Fix it!'), 'admin/config/user-interface/tenon'),
    );
    watchdog('tenon_api', 'Page report: API request attempt without credentials.');
  }
  // Verify that we have an URL to test.
  else if (empty($_GET['url'])) {
    $content = array(
      'content' => '<p>' . t('The page URL to test is not defined.') . '</p>',
    );
  }
  else {
    // Get the URL to test.
    $tested_url = $_GET['url'];

    // Build the query for the API.
    list($endpoint_url, $options) = tenon_query_build($tested_url, $api_key);
    $request = drupal_http_request($endpoint_url, $options);

    // Check if the request went through without any trouble.
    if ($request->code == 200) {
      // If we have a valid answer, decode the data.
      $data = drupal_json_decode($request->data);
      $output = tenon_query_display_page_results_summary($tested_url, $data);
      $content = array(
        'content' => $output,
        'link' => l(t('View full report'), 'https://tenon.io/history.php?responseID=' . $data['request']['responseID']),
      );
    }
    else {
      // Otherwise, explain the issue to the user and log it.
      $data = tenon_query_handle_errors_ajax($request);
      $content = array(
        'content' => $data['content'],
        'link' => $data['link'],
        'message_type' => $data['message_type'],
      );
    }
  }
  drupal_json_output($content);
  drupal_exit();
}

/**
 * Helper to build the query call.
 *
 * @param $tested_url
 *   URL to test.
 * @param $api_key
 *   API key.
 *
 * @return
 *   Array of data composed of:
 *   - Target URL against which the call is made.
 *   - Options to send to the request.
 */
function tenon_query_build($tested_url, $api_key) {
  $i = 0;
  $data = '';
  $api_parameters = array(
    'key' => $api_key,
    'url' => $tested_url,
    'appId' => variable_get('tenon_app_id', ''), // Remove if empty?
    'projectID' => variable_get('tenon_project_id', ''), // Remove if empty?
    'certainty' => variable_get('tenon_certainty', 0),
    'importance' => variable_get('tenon_importance', 0),
    'level' => variable_get('tenon_level', 'AAA'),
    'priority' => variable_get('tenon_priority', 0),
    'ref' => variable_get('tenon_ref', TRUE),
    'store' => TRUE,
    'uaString' => variable_get('tenon_ua_string', ''),
    'viewPortHeight' => variable_get('tenon_viewport_height', TENON_VIEWPORT_DEFAULT_HEIGHT),
    'viewPortWidth' => variable_get('tenon_viewport_width', TENON_VIEWPORT_DEFAULT_WIDTH),
  );
  foreach ($api_parameters as $parameter_name => $parameter_value) {
    $separator = '';
    if ($i++ != 0) {
      $separator = '&';
    }
    $data .= $separator . $parameter_name . '=' . urlencode($parameter_value);
  }

  // Query the API.
  $endpoint_url = variable_get('tenon_api_endpoint_page', TENON_IO_API_URL);
  $options = array(
    'headers' => array(
      'Content-Type' => 'application/x-www-form-urlencoded',
    ),
    'method' => 'POST',
    'data' => $data,
  );
  return array($endpoint_url, $options);
}

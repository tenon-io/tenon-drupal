<?php

namespace Drupal\tenon_io\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Configure the Tenon.io module.
 */
class AdminSettings extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'tenon_io_admin_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['tenon_io.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('tenon_io.settings');
    $integer_scale = [0, 20, 40, 60, 80, 100];

    $form['api_settings'] = [
      '#title' => $this->t('Tenon.io API settings'),
      '#type' => 'fieldset',
    ];
    $form['api_settings']['api_endpoint'] = [
      '#title' => $this->t('API endpoint URL'),
      '#description' => $this->t('URL of the endpoint for a single page test. Default url is %default_url.', ['%default_url' => TENON_IO_API_URL]),
      '#type' => 'url',
      '#required' => TRUE,
      '#default_value' => $config->get('api.endpoint'),
    ];
    $form['api_settings']['api_key'] = [
      '#title' => $this->t('API key'),
      '#description' => $this->t('You can find your key in your dashboard in the "API Key" section.'),
      '#type' => 'textfield',
      '#required' => TRUE,
      '#default_value' => $config->get('api.key'),
    ];
    $form['api_advanced_settings'] = [
      '#title' => $this->t('Tenon.io advanced API settings'),
      '#type' => 'details',
    ];
    $form['api_advanced_settings']['app_id'] = [
      '#title' => $this->t('App ID'),
      '#description' => $this->t('You can find your app ID in your dashboard after creating it in the "Applications" section.'),
      '#type' => 'textfield',
      '#default_value' => $config->get('app_id'),
    ];
    $form['api_advanced_settings']['certainty'] = [
      '#title' => $this->t('Certainty'),
      '#description' => $this->t('The certainty parameter allows you to filter out these "uncertain" results by choosing a minimum acceptable certainty score.'),
      '#type' => 'radios',
      '#default_value' => $config->get('certainty'),
      '#options' => array_combine($integer_scale, $integer_scale),
    ];
    $form['api_advanced_settings']['importance'] = [
      '#title' => $this->t('Importance'),
      '#description' => $this->t('This parameter is used when calculating final issue priority in the results. The importance parameter is only really relevant or useful when compiling a report set consisting of multiple documents.'),
      '#type' => 'radios',
      '#default_value' => $config->get('importance'),
      '#options' => [
        TENON_IO_IMPORTANCE_NONE => $this->t('None'),
        TENON_IO_IMPORTANCE_LOW => $this->t('Low'),
        TENON_IO_IMPORTANCE_MEDIUM => $this->t('Medium'),
        TENON_IO_IMPORTANCE_HIGH => $this->t('High'),
      ],
    ];
    $form['api_advanced_settings']['level'] = [
      '#title' => $this->t('Level'),
      '#description' => $this->t('The level parameter indicates the "lowest" WCAG level to test against.'),
      '#type' => 'radios',
      '#default_value' => $config->get('level'),
      '#options' => array_combine(['AAA', 'AA', 'A'], ['AAA', 'AA', 'A']),
    ];
    $form['api_advanced_settings']['priority'] = [
      '#title' => $this->t('Priority'),
      '#description' => $this->t('The priority parameter indicates the "lowest" issue priority to test against.'),
      '#type' => 'radios',
      '#default_value' => $config->get('priority'),
      '#options' => array_combine($integer_scale, $integer_scale),
    ];
    $form['api_advanced_settings']['ref_information'] = [
      '#title' => $this->t('Reference information'),
      '#description' => $this->t('Generate a link with each issue which includes reference information for the violated Best Practice.'),
      '#type' => 'checkbox',
      '#default_value' => $config->get('ref_information'),
    ];
    // The following field is hidden to force the setting. It's kept to be
    // configurable in the future.
    $form['api_advanced_settings']['store_information'] = [
      '#title' => $this->t('Store information'),
      '#description' => $this->t('Store your test results on Tenon to come back later and view your results in our Web-based system.'),
      // '#type' => 'radios',
      // '#default_value' => $config->get('store_information'),
      // '#options' => [0 => $this->t('No'), 1 => $this->t('Yes')],
      '#type' => 'value',
      '#value' => $config->get('store_information'),
    ];
    $form['api_advanced_settings']['project_id'] = [
      '#title' => $this->t('Project ID'),
      '#description' => $this->t('String of text you can supply to identify the tested document as part of a specific system. This is especially useful if you are developing or testing multiple projects.'),
      '#type' => 'textfield',
      '#default_value' => $config->get('project_id'),
      '#maxlength' => 255,
      '#size' => 20,
    ];
    $form['api_advanced_settings']['browser_user_agent'] = [
      '#title' => $this->t('User-Agent string'),
      '#description' => $this->t('Arbitrary string of text which will be sent as the User-Agent string when we request the URL you supply. This is particularly useful if your site does any user-agent sniffing.'),
      '#type' => 'textfield',
      '#default_value' => $config->get('browser.user_agent'),
      '#maxlength' => 255,
      '#size' => 50,
    ];
    $form['api_advanced_settings']['browser_width'] = [
      '#title' => $this->t('Viewport width'),
      '#description' => $this->t('Width, in pixels, of the viewport for our headless browser. The value must between 0 and 9999.'),
      '#type' => 'number',
      '#required' => TRUE,
      '#default_value' => $config->get('browser.width'),
      '#min' => 0,
      '#max' => 9999,
    ];
    $form['api_advanced_settings']['browser_height'] = [
      '#title' => $this->t('Viewport height'),
      '#description' => $this->t('Height, in pixels, of the viewport for our headless browser. The value must between 0 and 9999.'),
      '#type' => 'number',
      '#required' => TRUE,
      '#default_value' => $config->get('browser.height'),
      '#min' => 0,
      '#max' => 9999,
    ];

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $this->config('tenon_io.settings')
      ->set('api.endpoint', $form_state->getValue('api_endpoint'))
      ->set('api.key', $form_state->getValue('api_key'))
      ->set('app_id', $form_state->getValue('app_id'))
      ->set('certainty', $form_state->getValue('certainty'))
      ->set('importance', $form_state->getValue('importance'))
      ->set('level', $form_state->getValue('level'))
      ->set('priority', $form_state->getValue('priority'))
      ->set('ref_information', $form_state->getValue('ref_information'))
      ->set('store_information', $form_state->getValue('store_information'))
      ->set('project_id', $form_state->getValue('project_id'))
      ->set('browser.user_agent', $form_state->getValue('browser_user_agent'))
      ->set('browser.width', $form_state->getValue('browser_width'))
      ->set('browser.height', $form_state->getValue('browser_height'))
      ->save();
    parent::submitForm($form, $form_state);
  }

}

<?php

namespace Drupal\tenon_io\Service;

use Drupal\Component\Serialization\Json;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Config\ConfigException;
use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Core\Url;
use Drupal\tenon_io\TenonIoResponseException;
use GuzzleHttp\Client;
use GuzzleHttp\Exception\ClientException;
use Psr\Http\Message\ResponseInterface;

/**
 * TenonIo service.
 */
class TenonIo implements TenonIoInterface {
  use StringTranslationTrait;

  /**
   * Tenon.io config object.
   *
   * @var \Drupal\Core\Config\ImmutableConfig
   */
  protected $config;

  /**
   * Guzzle HTTP client.
   *
   * @var \GuzzleHttp\Client
   */
  protected $httpClient;

  /**
   * Default cache backend.
   *
   * @var \Drupal\Core\Cache\CacheBackendInterface
   */
  protected $cache;

  /**
   * TenonIo constructor.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $configFactory
   *   The config factory to get the settings from.
   * @param \GuzzleHttp\Client $client
   *   The Guzzle HTTP client to make requests to the API.
   * @param \Drupal\Core\Cache\CacheBackendInterface $cache
   *   The default cache backend to store data.
   */
  public function __construct(ConfigFactoryInterface $configFactory, Client $client, CacheBackendInterface $cache) {
    $this->config = $configFactory->get('tenon_io.settings');
    $this->httpClient = $client;
    $this->cache = $cache;
  }

  /**
   * {@inheritdoc}
   */
  public function getIssuesCountFromCache($url) {
    $issues_count_cache = $this->cache->get('tenon_io_' . sha1($url));
    if ($issues_count_cache) {
      $issues_count_cache = $issues_count_cache->data;
    }
    return $issues_count_cache;
  }

  /**
   * {@inheritdoc}
   */
  public function setIssuesCountToCache($url, $issuesCount) {
    $this->cache->set('tenon_io_' . sha1($url), $issuesCount);
  }

  /**
   * {@inheritdoc}
   */
  public function checkPage($url) {
    $endpoint = $this->getQueryEndpoint();
    $data = $this->getQueryData();
    $data['url'] = $url;

    $response = $this->queryApi($endpoint, $data);

    return Json::decode($response->getBody()->getContents());
  }

  /**
   * Sends the request to the API.
   *
   * @param string $endpoint
   *   The API endpoint.
   * @param array $data
   *   The data to send to the API.
   *
   * @return \Psr\Http\Message\ResponseInterface
   *   The API response object.
   */
  protected function queryApi($endpoint, $data) {
    try {
      return $this->httpClient->post($endpoint, ['form_params' => $data]);
    }
    catch (ClientException $e) {
      $this->handleResponse($e->getResponse());
    }
  }

  /**
   * Handles a response to figure out if an error occured.
   *
   * @param \Psr\Http\Message\ResponseInterface $response
   *   The response.
   *
   * @throws \Drupal\tenon_io\TenonIoResponseException
   *   An exception explaining what happened.
   */
  protected function handleResponse(ResponseInterface $response) {
    $message = '';
    $message_type = 'error';
    $message_variables = [];
    $more_info = [];
    if (empty($response->getBody()->getSize())) {
      $message = 'The connection to the resource timed out.';
      $data = ['status' => 0];
    }
    else {
      $data = Json::decode($response->getBody()->getContents());
      switch ($data['status']) {
        case 200:
          // Nothing to do in here.
          break;

        case 400:
          $message = 'The request is malformed.';

          // Identify the cause of the unauthorized access.
          switch ($data['code']) {
            case 'invalid_param':
              $message_type = 'warning';
              $message = '@invalid_param_message. Please verify your settings.';
              $message_variables = [
                '@invalid_param_message' => $data['message'],
              ];
              $more_info[] = [
                'title' => $this->t('Access the settings page'),
                'url' => Url::fromRoute('tenon_io.admin_settings'),
              ];
              break;

            case 'blank_url_or_src':
              $message = '@invalid_param_message. Please verify the tested URL.';
              $message_variables = [
                '@invalid_param_message' => $data['message'],
              ];
              break;

            case 'bad_src':
              $message = '@invalid_param_message. Please verify the tested SRC.';
              $message_variables = [
                '@invalid_param_message' => $data['message'],
              ];
              break;

            case 'abuse_detected':
              $message = 'An abuse has been detected.';
              break;

            case 'required_param_missing':
              $message = '@invalid_param_message';
              $message_variables = [
                '@invalid_param_message' => $data['message'],
              ];
              break;

            case 'doc_source_too_big':
              $message = 'The content submitted is too big to be analyzed.';
              break;

            case 'improper_content_type':
              $message = 'The submitted resource does not appear to be a valid content type.';
              break;

            case 'url_request_failed':
              $message = 'The attempt to test the requested URL failed.';
              break;

          }
          break;

        case 401:
          // Identify the cause of the unauthorized access.
          switch ($data['code']) {
            case 'api_credentials_invalid':
              $message_type = 'warning';
              $message = 'Please verify your API credentials.';
              $more_info[] = [
                'title' => $this->t('Verify the settings page'),
                'url' => Url::fromRoute('tenon_io.admin_settings'),
              ];
              break;

            case 'monthly_limit_reached':
              $message = 'It looks like you have reached your monthly limit.';
              break;

            default:
              $message = 'The connection to the API is unauthorized.';
              break;

          }
          break;

        case 500:
          $message = 'An internal error occurred, Please try to submit your request again.';
          break;

        case 522:
          $message = 'The connection to the resource timed out.';
          break;

        default:
          $message = 'An error occured.';
          break;

      }
    }

    if (!empty($message)) {
      if (!empty($data['moreInfo'])) {
        $more_info[] = [
          'title' => $this->t('View the error details'),
          'url' => Url::fromUri($data['moreInfo']),
        ];
      }
      throw new TenonIoResponseException($this->t($message, $message_variables), $message_type, $more_info, $data['status']);
    }
  }

  /**
   * Helper to build the query data.
   *
   * @return array
   *   Array of data composed of:
   *   - Options to send to the request.
   *
   * @throws ConfigException
   *   If the API key is not defined.
   */
  protected function getQueryData() {
    $apiKey = $this->config->get('api.key');
    if (empty($apiKey)) {
      throw new ConfigException('API key missing');
    }

    return [
      'key' => $apiKey,
      'appId' => $this->config->get('app_id'),
      'projectID' => $this->config->get('project_id'),
      'certainty' => $this->config->get('certainty'),
      'importance' => $this->config->get('importance'),
      'level' => $this->config->get('level'),
      'priority' => $this->config->get('priority'),
      'ref' => $this->config->get('ref_information'),
      'store' => $this->config->get('store_information'),
      'uaString' => $this->config->get('browser.user_agent'),
      'viewPortHeight' => $this->config->get('browser.height'),
      'viewPortWidth' => $this->config->get('browser.width'),
    ];
  }

  /**
   * Getter for the API endpoint setting.
   *
   * @return string
   *   The API endpoint URL.
   */
  protected function getQueryEndpoint() {
    return $this->config->get('api.endpoint');
  }

}

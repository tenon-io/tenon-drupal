<?php

namespace Drupal\tenon_io\Controller;

use Drupal\Core\Ajax\AjaxResponse;
use Drupal\Core\Config\ConfigException;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\tenon_io\Service\TenonIoInterface;
use Drupal\tenon_io\TenonIoResponseException;
use Psr\Log\LoggerInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpKernel\Exception\AccessDeniedHttpException;

/**
 * Tenon.io reports controller.
 */
class ReportController extends ControllerBase {

  /**
   * The TenonIo service.
   *
   * @var \Drupal\tenon_io\Service\TenonIoInterface
   */
  protected $tenonIo;

  /**
   * The TenonIo logger.
   *
   * @var \Psr\Log\LoggerInterface
   */
  protected $logger;

  /**
   * TenonIo reports controller constructor.
   *
   * @param \Drupal\tenon_io\Service\TenonIoInterface $tenonIo
   *   The TenonIo service.
   * @param \Psr\Log\LoggerInterface $logger
   *   The TenonIo logger.
   */
  public function __construct(TenonIoInterface $tenonIo, LoggerInterface $logger) {
    $this->tenonIo = $tenonIo;
    $this->logger = $logger;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('tenon_io'),
      $container->get('logger.channel.tenon_io')
    );
  }

  /**
   * Tenon.io report page.
   *
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The HTTP request.
   *
   * @return array
   *   Page render array.
   */
  public function page(Request $request) {
    $tested_url = $request->query->get('url');
    if (empty($tested_url)) {
      $tested_url = Url::fromRoute('<current>', [], ['absolute' => TRUE])->toString();
    }

    try {
      $data = $this->tenonIo->checkPage($tested_url);
    }
    catch (ConfigException $e) {
      // The user has not provided they API key. Invite they to do so.
      drupal_set_message($this->t('Unfortunately we are not able to generate a report untill you <a href=":api_settings">provide us an API key!</a>', [':api_settings' => Url::fromRoute('tenon_io.admin_settings')->toString()]), 'warning');
      $this->logger->warning('Page report: API request attempt without credentials.');
      throw new AccessDeniedHttpException();
    }
    catch (TenonIoResponseException $e) {
      // Display a message to the user and log the error.
      drupal_set_message($e->getMessage(), $e->getMessageType());
      $this->logger->{$e->getMessageType()}('Error !code: !error.', [
        '!code' => $e->getCode(),
        '!message' => $e->getMessage(),
      ]);

      $build = [
        'result' => [
          // @TODO: improve this user message.
          '#markup' => '<p>' . $this->t('An error occurred, we deeply apologize.') . '</p>',
        ],
      ];

      // If we have a link to provide with more explanations of the cause of
      // the error, provide it to the user.
      if (!empty($e->getMoreInfo())) {
        $build['more_info'] = [
          '#theme' => 'links',
          '#links' => $e->getMoreInfo(),
        ];
        drupal_set_message($build['more_info'], $e->getMessageType());
      }

      return $build;
    }

    // Extract the results and format the response for the end user.
    $issue_count = $data['resultSummary']['issues']['totalIssues'];
    $error_count = $data['resultSummary']['issues']['totalErrors'];
    $warning_count = $data['resultSummary']['issues']['totalWarnings'];
    $report_url = 'https://tenon.io/history.php?responseID=' . $data['request']['responseID'];

    // Store the number of issues for a given URL.
    $this->tenonIo->setIssuesCountToCache($tested_url, $issue_count);

    return [
      '#theme' => 'tenon_results_report_full',
      '#report_url' => Link::fromTextAndUrl($this->t('View full results on Tenon.io'), Url::fromUri($report_url))->toRenderable(),
      '#error_count' => $error_count,
      '#warning_count' => $warning_count,
      '#issues_count' => $issue_count,
      '#a_level_count' => $data['resultSummary']['issuesByLevel']['A']['count'],
      '#a_level_percentage' => $data['resultSummary']['issuesByLevel']['A']['pct'],
      '#aa_level_count' => $data['resultSummary']['issuesByLevel']['AA']['count'],
      '#aa_level_percentage' => $data['resultSummary']['issuesByLevel']['AA']['pct'],
      '#aaa_level_count' => $data['resultSummary']['issuesByLevel']['AAA']['count'],
      '#aaa_level_percentage' => $data['resultSummary']['issuesByLevel']['AAA']['pct'],
      '#tested_url' => $tested_url,
    ];
  }

  /**
   * Tenon.io ajax report page.
   *
   * @param \Symfony\Component\HttpFoundation\Request $request
   *   The HTTP request.
   *
   * @return AjaxResponse
   *   The Ajax response object.
   */
  public function ajaxPage(Request $request) {
    $tested_url = $request->query->get('url');
    if (empty($tested_url)) {
      $content = [
        'content' => '<p>' . $this->t('The page URL to test is not defined.') . '</p>',
      ];
    }

    try {
      $data = $this->tenonIo->checkPage($tested_url);
    }
    catch (ConfigException $e) {
      // The user has not provided they API key. Invite they to do so.
      $content = [
        'content' => '<p>' . $this->t('Unfortunately we are not able to generate a report untill you provide an API key!') . '</p>',
        'link' => Link::createFromRoute($this->t('Provide an API key'), 'tenon_io.admin_settings')->toString(),
      ];
      $this->logger->warning('Page report: API request attempt without credentials.');
      return new AjaxResponse($content);
    }
    catch (TenonIoResponseException $e) {
      // Display a message to the user and log the error.
      $this->logger->{$e->getMessageType()}('Error !code: !error.', [
        '!code' => $e->getCode(),
        '!message' => $e->getMessage(),
      ]);

      $content = [
        'content' => '<p>' . $this->t('An error occurred, we deeply apologize.') . '</p>' . $e->getMessage(),
        'message_type' => $e->getMessageType(),
        'link' => '',
      ];

      // If we have a link to provide with more explanations of the cause of
      // the error, provide it to the user.
      if (!empty($e->getMoreInfo())) {
        foreach ($e->getMoreInfo() as $link) {
          $content['link'] .= Link::fromTextAndUrl($link['title'], $link['url'])->toString();
        }
      }

      return new AjaxResponse($content);
    }

    // Extract the results and format the response for the end user.
    $issue_count = $data['resultSummary']['issues']['totalIssues'];
    $error_count = $data['resultSummary']['issues']['totalErrors'];
    $warning_count = $data['resultSummary']['issues']['totalWarnings'];
    $report_url = 'https://tenon.io/history.php?responseID=' . $data['request']['responseID'];

    // Store the number of issues for the tested URL.
    $this->tenonIo->setIssuesCountToCache($tested_url, $issue_count);

    $content = [
      'content' => [
        '#theme' => 'tenon_results_report_summary',
        '#error_count' => $error_count,
        '#warning_count' => $warning_count,
        '#issues_count' => $issue_count,
      ],
      'link' => Link::fromTextAndUrl($this->t('View full results on Tenon.io'), Url::fromUri($report_url))->toString(),
    ];
    return new AjaxResponse($content);
  }

}

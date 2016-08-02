/**
 * @file
 * TenonIo javascript behaviors.
 */

(function ($, Drupal, settings, hopscotch) {
  'use strict';

  /**
   * Reacts on Tenon.io toolbar button click.
   */
  Drupal.behaviors.tenon_toolbar = {
    attach: function (context) {
      var $trigger = $('.js-tenon-io-trigger');

      // Only continue if we have the hopscotch library defined and if the
      // toolbar displays the "Accessibility check" button.
      if (typeof hopscotch === 'undefined' || $trigger.length === 0) {
        return;
      }

      // Display the potential existing page results of a previous test.
      Drupal.tenon.generateTestReport(settings, '.tenon-io-toolbar-tab');

      // Adds our tour overlay behavior with desired effects.
      $trigger.click(function (e) {
        e.preventDefault();
        e.stopPropagation();

        // Sets up the tour object with appropriate content.
        Drupal.tenon.settings.placement = 'bottom';
        Drupal.tenon.settings.arrowOffset = 'center';
        Drupal.tenon.settings.xOffset = -100;

        // Build the report content for the tested page.
        Drupal.tenon.settlePageReportGeneration(settings);
      });
    }
  };

  /**
   * Drupal tenon library helper.
   */
  Drupal.tenon = Drupal.tenon || {
    // Default settings for the report content container.
    settings: {
      title: Drupal.t('Your page accessibility report'),
      placement: 'bottom',
      yOffset: -3,
      xOffset: -100,
      arrowOffset: 'center'
    },

    // Displays the results of a previous test.
    generateTestReport: function (settings, selector) {
      // If we have a count of issues reported for the current page, show it.
      if (settings.tenon_io.issuesCount !== false && settings.tenon_io.issuesCount !== null) {
        // Format the message based on its structure.
        var issues_class = '';
        if (settings.tenon_io.issuesCount === 0) {
          issues_class = 'no-issue';
        }
        else {
          issues_class = 'has-issue';
        }
        // Display the count of issues from the previous test.
        var summary_count_placeholder = $('#tenon-report-summary-count');
        if (summary_count_placeholder.length === 0) {
          $(selector).append($('<span id="tenon-report-summary-count" class="' + issues_class + '">' + settings.tenon_io.issuesCount + '</span>'));
        }
        else {
          $(summary_count_placeholder).html(settings.tenon_io.issuesCount).removeClass('no-issue').removeClass('has-issue').addClass(issues_class);
        }
      }
    },

    // Builds the report content and send the current page to an AJAX call
    // to get the results.
    settlePageReportGeneration: function (settings) {
      var tour = {
        showPrevButton: true,
        scrollTopMargin: 100,
        id: 'tenon-notifications',
        steps: [Drupal.tenon.buildItem(settings.tenon_io.moduleBasePath)]
      };
      // Adjust the rendering to match our needs.
      var hopscotch_selector = '.hopscotch-bubble';
      $(hopscotch_selector).addClass('animated');
      hopscotch.startTour(tour);

      // Trigger the page report for the current URL.
      // Format the URL according to the fact that the
      // URL rewriting is on or off.
      var url = Drupal.url('tenon_io/ajax/page?url=' + encodeURI(settings.tenon_io.url));
      $.get(url).done(function (data) {
        $('.tenon-notifications .hopscotch-content .description').html(data.content);
        $('.tenon-notifications .hopscotch-content .tenon-notifications-readmore').html(data.link);
      });
      // Removes animation for each step and let us to target just this tour in CSS rules.
      $(hopscotch_selector).removeClass('animated').addClass('tenon-notifications');
    },

    // Formats tour content.
    buildItem: function (module_path) {
      // Prepare the output to display inside the tour's content region.
      // Builds a default message to show the user that he has to wait.
      var output = '<div class="feed_item">';
      output += '<span class="description"><p>' + Drupal.t('Report generation in progress.') + '<br />';
      output += '<img src="' + module_path + '/images/throbber.gif" /></p></span>';
      output += '<div class="tenon-notifications-readmore"></div>';
      output += '</div>';

      Drupal.tenon.settings.content = output;
      Drupal.tenon.settings.target = document.querySelector('.tenon-io-toolbar-tab');
      // Returns the item to be added to the tour's (array) `items` property.
      return Drupal.tenon.settings;
    }
  };

})(jQuery, Drupal, drupalSettings, hopscotch);

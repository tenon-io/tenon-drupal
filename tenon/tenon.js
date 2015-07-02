/**
 * jQuery behaviors for tenon reports.
 */
(function ($) {
  Drupal.behaviors.tenon_reports = {
    attach: function (context, settings) {
      // Setup.
      var menuLinkSel = 'a[data-tenon="page_report"]';
      if ($(menuLinkSel + '.tenon-notifications-processed').length) {
        return;
      }
      $(menuLinkSel).addClass('tenon-notifications-processed');

      // Only continue if we have the hopscotch library defined.
      if (typeof hopscotch == 'undefined') {
        return;
      }
      // If we have a count of issues reported for the current page, show it.
      if (settings.tenon.issuesCount !== false) {
        // Format the message based on its structure.
        var issues_class = '';
        if (settings.tenon.issuesCount === 0) {
          issues_class = 'no-issue';
        }
        else {
          issues_class = 'has-issue';
        }
        // Display the count of issues from the previous test.
        $('.tenon-link').append($('<span id="tenon-report-summary-count" class="' + issues_class + '">' + settings.tenon.issuesCount + '</span>'));
      }

      // Adds our tour overlay behavior with desired effects.
      $('a[data-tenon="page_report"]').click(function (e) {
        e.preventDefault();
        e.stopPropagation();

        // Close the drawer when we generate the report.
        Drupal.cp_toolbar.drawer_close();

        // Sets up the tour object with appropriate content.
        var tour = {
          showPrevButton: true,
          scrollTopMargin: 100,
          id: "tenon-notifications",
          steps: [Drupal.tenon.buildItem(settings.tenon.moduleBasePath)]
        };
        // Adjust the rendering to match our needs.
        var hopscotch_selector = '.hopscotch-bubble';
        $(hopscotch_selector).addClass('animated');
        hopscotch.startTour(tour);

        // Trigger the page report for the current URL.
        // Format the URL according to the fact that the
        // URL rewriting is on or off.
        if (settings.tenon.cleanUrl == true) {
          var url = Drupal.settings.basePath + 'tenon/ajax/page?url=' + encodeURI(settings.tenon.url);
        }
        else {
          var url = Drupal.settings.basePath + '?q=tenon/ajax/page&url=' + encodeURI(settings.tenon.url);
        }
        $.get(url).done(function(data) {
          $('.tenon-notifications .hopscotch-content .description').html(data.content);
          $('.tenon-notifications .hopscotch-content .tenon-notifications-readmore').html(data.link);
        });

        // Removes animation for each step and let us to target just this tour in CSS rules.
        $(hopscotch_selector).removeClass('animated').addClass('tenon-notifications');
      });
    }
  };

  Drupal.tenon = Drupal.tenon || {
    /**
     * Format Tour content.
     *
     * @returns {{title: string, content: string, target: Element, placement: string, yOffset: number, xOffset: number}}
     */
    buildItem: function (module_path) {
      // Prepare the output to display inside the tour's content region.
      // Builds a default message to show the user that he has to wait.
      var output = "<div class='feed_item'>";
      output += '<span class="description"><p>' + Drupal.t("Report generation in progress.") + '<br />';
      output += '<img src="' + module_path + '/throbber.gif" /></p></span>';
      output += '<div class="tenon-notifications-readmore"></div>';
      output += '</div>';

      // Returns the item to be added to the tour's (array) `items` property.
      return {
        title: Drupal.t('Your page accessibility report'),
        content: output,
        target: document.querySelector('.tenon-link'),
        placement: "bottom",
        yOffset: -3,
        xOffset: -10,
      };
    }
  };
})(jQuery);

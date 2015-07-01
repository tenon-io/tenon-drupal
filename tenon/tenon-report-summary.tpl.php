<?php if ($issues_count > 0) : ?>
  <h5><?php print t('Result Summary:'); ?></h5>
  <ul>
    <?php if ($error_count > 0) : ?>
      <li>
        <strong><?php print t('Total Errors:'); ?></strong> <?php print $error_count; ?>
      </li>
    <?php endif; ?>
    <?php if ($warning_count > 0) : ?>
      <li>
        <strong><?php print t('Total Warnings:'); ?></strong> <?php print $warning_count; ?>
      </li>
    <?php endif; ?>
  </ul>
<?php else: ?>
  <p>
    <?php print t('Congratulations, you do not have any warning or error on your page.'); ?>
  </p>
<?php endif; ?>

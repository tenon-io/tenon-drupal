<?php if ($issues_count > 0) : ?>
  <h2><?php print t('Result Summary: (!report_url)', array('!report_url' => $report_url)); ?></h2>
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
  <h3><?php print t('Issues by Level'); ?></h3>
  <table>
    <thead>
    <tr>
      <th scope="col"><?php print t('Level'); ?></th>
      <th scope="col"><?php print t('Count'); ?></th>
      <th scope="col">%</th>
    </tr>
    </thead>
    <tbody>
    <tr>
      <th scope="row">A</th>
      <td><?php print $a_level_count; ?></td>
      <td><?php print $a_level_percentage; ?>%</td>
    </tr>
    <tr>
      <th scope="row">AA</th>
      <td><?php print $aa_level_count; ?></td>
      <td><?php print $aa_level_percentage; ?>%</td>
    </tr>
    <tr>
      <th scope="row">AAA</th>
      <td><?php print $aaa_level_count; ?></td>
      <td><?php print $aaa_level_percentage; ?>%</td>
    </tr>
    </tbody>
  </table>
<?php else: ?>
  <p>
    <?php print t('Congratulations, you do not have any warning or error on your page <strong>!tested_url</strong>.', array('!tested_url' => $tested_url)); ?>
  </p>
<?php endif; ?>

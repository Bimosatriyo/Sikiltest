<?php

/**
 * @file
 */
?>
<div class="<?php print $classes; ?>">
  <div class="achievement-image"><?php print render($image); ?></div>

  <div class="achievement-points-box">
    <div class="achievement-points"><?php print $achievement['points']; ?></div>
    <div class="achievement-unlocked-stats">
      <div class="achievement-unlocked-rank"><?php print render($unlocked_rank); ?></div>
    </div>
  </div>

  <div class="achievement-body">
    <div class="achievement-header"><?php print t('Achievement Unlocked'); ?></div>
    <div class="achievement-title"><?php print render($achievement_title); ?></div>
  </div>
</div>

<?php

/**
 * @file
 */
?>
<div class="<?php print $classes; ?>">
  <?php print render($title_suffix); ?>
  <div class="achievement-image"><?php print render($image); ?></div>

  <div class="achievement-points-box">
    <div class="achievement-points"><?php print $achievement['points']; ?></div>
    <div class="achievement-unlocked-stats">
      <div class="achievement-unlocked-timestamp"><?php print render($unlocked_date); ?></div>
      <div class="achievement-unlocked-rank"><?php print render($unlocked_rank); ?></div>
    </div>
  </div>

  <div class="achievement-body">
    <div class="achievement-title"><?php print render($achievement_title); ?></div>
    <div class="achievement-description"><?php print $achievement['description']; ?></div>
  </div>
</div>

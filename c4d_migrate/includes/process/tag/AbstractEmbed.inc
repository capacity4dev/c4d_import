<?php
/**
 * @file
 * Base tag processor.
 */

/**
 * Abstract tag processor restore embed codes from tags.
 */
abstract class C4dMigrateProcessTagAbstractEmbed extends C4dMigrateProcessTagAbstract {
  /**
   * The information about the embed code we received from the old DB.
   *
   * @var object
   */
  private $info;


  /**
   * Set the info received from the DB.
   *
   * @param object $info
   */
  protected function setInfo($info) {
    $this->info = $info;
  }

  /**
   * Get the info as retrieved from the DB.
   *
   * @return object
   */
  public function getInfo() {
    return $this->info;
  }

  /**
   * {@inheritdoc}
   */
  public function process() {
    $tag = $this->getTag();

    $info = $this->loadInfo($tag);
    if (!$info) {
      return FALSE;
    }

    $this->setInfo($info);
    return TRUE;
  }

  /**
   * Every processor needs to define how to retrieve the information.
   *
   * @param array $tag
   *   The tag information.
   *
   * @return stdClass
   */
  abstract protected function loadInfo($tag);

  /**
   * Helper to set the dimensions on the embed code.
   *
   * @param string $code
   *   The embed code.
   * @param int $width
   *   The desired width.
   * @param int $height
   *   The desired height.
   *
   * @return string
   *   The code with filled in dimensions.
   */
  protected function setDimensions($code, $width, $height) {
    if (preg_match('/width="[0-9]+"/iSu', $code)) {
      $code = preg_replace('/width="[0-9]+"/iSu', 'width="' . $width . '"', $code);
    }
    if (preg_match('/height="[0-9]+"/iSu', $code)) {
      $code = preg_replace('/height="[0-9]+"/iSu', 'height="' . $height . '"', $code);
    }

    return $code;
  }

}

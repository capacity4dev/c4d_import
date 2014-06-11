<?php
/**
 * @file
 * Helper class to scan body content for media and import them to the media lib.
 */

/**
 * Helper class to convert old style media tags to new style media tags.
 */
class C4dMigrateProcessBodyMedia {
  /**
   * The body content that needs processing.
   *
   * @var string
   */
  private $body;

  /**
   * The mapping between strings we need to replace and their replacements.
   *
   * @var array
   */
  private $replacements;


  /**
   * Construct by passing the body data.
   *
   * @param string $body
   *   The body content.
   */
  public function __construct($body) {
    $this->setBody($body);
    $this->replacements = array();
  }

  /**
   * Set the body we need to process.
   *
   * @param string $body
   */
  private function setBody($body) {
    $this->body = $body;
  }

  /**
   * Get the original body.
   *
   * @return string
   */
  public function getBody() {
    return $this->body;
  }

  /**
   * Get the file id's that are related to teh processed body.
   *
   * @return array
   */
  public function getFileIds() {
    return $this->fileIds;
  }

  /**
   * Add a single file id to the file usage array.
   *
   * @param int $fid
   *   The file id that needs to be added.
   */
  protected function addFileId($fid) {
    if (!in_array($fid, $this->fileIds)) {
      $this->fileIds[] = $fid;
    }
  }


  /**
   * Process the body.
   *
   * @return bool
   *   Was there anything we needed to process?
   */
  public function process() {
    $this->processC4dImage();
    $this->processIbImage();
    $this->processVideo();

    $file_ids = $this->getFileIds();
    return !empty($file_ids);
  }

  /**
   * Process the [c4d-image:…] tags.
   */
  private function processC4dImage() {
    $body = $this->getBody();

    // Identify all body parts.

    $this->setBody($body);
  }

  /**
   * Process the [ibimage] tags.
   */
  private function processIbImage() {
    $body = $this->getBody();



    $this->setBody($body);
  }

  /**
   * Process the [video:…] tags.
   */
  private function processVideo() {
    $body = $this->getBody();



    $this->setBody($body);
  }


}
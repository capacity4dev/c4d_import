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
   *   Body string (html) to process.
   */
  private function setBody($body) {
    $this->body = $body;
  }

  /**
   * Get the original body.
   *
   * @return string
   *   Body content.
   */
  public function getBody() {
    return $this->body;
  }


  /**
   * Process the body.
   */
  public function process() {
    $this->processC4dImage();
    $this->processIbImage();
    $this->processVideo();
  }

  /**
   * Process the [c4d-image:…] tags.
   */
  private function processC4dImage() {
    $body = $this->getBody();

    // Identify all body parts.
    $parser = new C4dMigrateParserC4dImage();
    $tags = $parser->parse($body);
    if (!$tags) {
      return;
    }

    // Process the found tags.


    // Replace the body tags.



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

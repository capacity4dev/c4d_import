<?php
/**
 * @file
 * Class to import data into comments.
 */

/**
 * Class to import data into comments for comments on content within groups.
 */
abstract class C4dMigrateImportBaseCommentOg extends C4dMigrateImportBaseComment {
  /**
   * Actions after the data is saved in the database.
   *
   * @param object $entity
   *   The entity that is saved into the platform.
   * @param object $row
   *   The data row we are importing.
   */
  public function complete($entity, $row) {
    // Make sure the messages have the proper timestamp.
    $this->messageInsertSave($entity, $row);
  }

}

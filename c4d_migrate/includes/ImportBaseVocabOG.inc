<?php
/**
 * @file
 * Base class to import OG Vocabs data from capacity4dev into capacity4more.
 */

/**
 * Base class to import OG Vocabs data from capacity4dev into capacity4more.
 */
abstract class C4dMigrateImportBaseVocabOG extends C4dMigrateImportBase {
  /**
   * Content creations need to be run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportNodeGroup',
    'C4dMigrateImportNodeProject',
  );

  /**
   * Retrieve the destination nid of the gid.
   *
   * @param string $type
   *   String containing the group type (machine name).
   * @param int    $gid
   *   The nid of the term's group we are importing.
   *
   * @return int
   *   The destination nid of the group.
   */
  protected function getMappedGroupGid($type, $gid) {
    switch ($type) {
      case 'project_group':
        $gid_mapped = $this->handleSourceMigration('C4dMigrateImportNodeProject', $gid, NULL, $this);
        break;
      case 'group':
      default:
        $gid_mapped = $this->handleSourceMigration('C4dMigrateImportNodeGroup', $gid, NULL, $this);
        break;
    }

    return $gid_mapped;
  }
}

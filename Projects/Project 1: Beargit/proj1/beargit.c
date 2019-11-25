#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);

  FILE* fbranches = fopen(".beargit/.branches", "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);
   
  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  write_string_to_file(".beargit/.current_branch", "master");

  return 0;
}



/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}

/* beargit status
 *
 * - Read the file .beargit/.index and print a line for each tracked file. 
 * Unlike git status, beargit status should not print anything about untracked files.
 *
 * Output (to stdout):
 * - Always return 0 (indicate success)
 *
 */

int beargit_status() {
  /* COMPLETE THE REST */
  fprintf(stdout, "Tracked files:\n\n");
  FILE* findex = fopen(".beargit/.index", "r");
  
  int count = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    fprintf(stdout, "  %s\n", line);
    ++count;
  }
  fprintf(stdout, "\n%d files total\n", count);
  fclose(findex);

  return 0;
}

/* beargit rm <filename>
 * 
 * The rm command in beargit takes in a single argument,
 * which specifies the file to remove from the index (which is stored in the file .beargit/.index)
 * Note that this behavior is different from git in that it doesn't delete the file from your file system.
 * 
 * Possible errors (to stderr):
 * >> ERROR: File <filename> not tracked
 *
 * Output (to stdout):
 * - None if successful and Otherwise, return 1
 * 
 */

int beargit_rm(const char* filename) {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  int hasfile = 0;

  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      hasfile = 1;
    } else {
      fprintf(fnewindex, "%s\n", line);
    }
  }

  if (!hasfile) {
      fprintf(stderr, "ERROR: File %s not tracked\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 1;
  }

  fclose(findex);
  fclose(fnewindex);
  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}

/* beargit commit -m <msg>
 *
 * The commit command involves a couple of steps:
 * First, check whether the commit string contains "GO BEARS!". If not, display an error message.
 * Read the ID of the previous last commit from .beargit/.prev
 * Generate the next ID (newid) in such a way that:
 * ID Length is COMMIT_ID_BYTES (not including NULL terminator)
 * All characters of the id are either 6, 1 or c
 * Generating 100 IDs in a row will generate 100 IDs that are all unique (Hint: you can do this in such a way that you go through all possible IDs before you repeat yourself. Some of the ideas from number representation may help you!)
 * Calling next_commit_id(char* commit_id) results in commit_id being updated to a ID.
 * The ID string consists of a branch-id (of size COMMIT_ID_BRANCH_BYTES) followed by a tag-id to fill the rest of the size of the ID. (Note: the tag-id used here has nothing to do with a git tag, git tags aren't involved in this project!)
 * We have implemented the branch-id step for you in next_commit_id(char* commit_id). Don't worry too much about where the branch-id is coming from yet (more on that in part 5), but pay close attention to what indices in the commit_id string are being updated and how the pointer is being passed to next_commit_id_part1(). To finish the next ID generation you will need to complete next_commit_id_part1().
 * Generate a new directory .beargit/<newid> and copy .beargit/.index, .beargit/.prev and all tracked files into the directory.
 * Store the commit message (<msg>) into .beargit/<newid>/.msg
 * Write the new ID into .beargit/.prev.
 * 
 * Possible errors (to stderr):
 * >> ERROR: Message must contain "GO BEARS!"
 *
 * Output (to stdout):
 * - If the commit message does not contain the exact string "GO BEARS!", return 1. Otherwise, return 0.
 */

const char* go_bears = "GO BEARS!";
const int MAX_LENGTH = 1000;

int is_commit_msg_ok(const char* msg) {
  /* COMPLETE THE REST */
  int msg_len = strlen(msg);
  int go_bears_len = strlen(go_bears);

  if (msg_len < go_bears_len) {
    return 0;
  }

  for (int i = 0; i < msg_len - go_bears_len + 1; ++i) {
    int is_ok = 1;
    for (int j = 0; j < go_bears_len; ++j) {
      if (msg[i + j] != go_bears[j]) {
        is_ok = 0;
        break;
      } 
    }
    if (is_ok) {
      return 1;
    }
  }

  return 0;
}

void next_commit_id_part1(char* commit_id) {
  /* COMPLETE THE REST */
  int part1_len = strlen(commit_id);
  for (int i = 0; i < part1_len; ++i) {
    char *c = commit_id + i;
    if (*c == '0') {
      *c = '6';
      continue;
    } else if (*c == '6') {
      *c = '1';
      return;
    } else if (*c == '1') {
      *c = 'c';
      return;
    } else {
      continue;
    }
  }
}

void move_alltracked_file(const char *new_dir_name) {
  
  FILE* findex = fopen(".beargit/.index", "r");
  char filename[FILENAME_SIZE];

  char line[FILENAME_SIZE];
  while(fgets(filename, sizeof(line), findex)) {
    strtok(filename, "\n");
    char copied_filename[MAX_LENGTH];
    sprintf(copied_filename,"%s/%s", new_dir_name, filename);

    fs_cp(filename, copied_filename);
  }
  fclose(findex);
}

int beargit_commit(const char* msg) {
  
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  if (!strlen(current_branch)) {
    fprintf(stderr, "ERROR: Need to be on HEAD of a branch to commit");
  }

  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id); 

  /* COMPLETE THE REST */
  char new_dir_name[MAX_LENGTH];
  sprintf(new_dir_name, ".beargit/%s", commit_id);
  fs_mkdir(new_dir_name);

  move_alltracked_file(new_dir_name);

  char copied_index_file[MAX_LENGTH];
  sprintf(copied_index_file, "%s/.index", new_dir_name);
  fs_cp(".beargit/.index", copied_index_file);

  char copied_prev_file[MAX_LENGTH];
  sprintf(copied_prev_file, "%s/.prev", new_dir_name);
  fs_cp(".beargit/.prev", copied_prev_file);

  char msg_file[MAX_LENGTH];
  sprintf(msg_file, "%s/.msg", new_dir_name);
  write_string_to_file(msg_file, msg);

  write_string_to_file(".beargit/.prev", commit_id);
  

  return 0;
}

/* beargit log
 *
 * - List all commits, latest to oldest. .beargit/.prev contains the ID of the latest commit, 
 * and each directory .beargit/ contains a .prev file pointing to that commit's predecessor.
 * - For each commit, print the commit's ID followed by the commit message (see below for the exact format).
 * - If you pass in the -n flag (e.g. beargit -n 10), then limit the number of log records printed to the amount specified. 
 *   If the -n flag is not passed, then the argument "int limit" will be set to INT_MAX.
 * 
 * Possible errors (to stderr):
 * >> ERROR: There are no commits!
 *
 * Output (to stdout):
 * - If there are commits return 0. Otherwise, return 1;
 * 
 */

int beargit_log(int limit) {
  /* COMPLETE THE REST */
  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  if (strcmp(commit_id, "0000000000000000000000000000000000000000") == 0) {
    fprintf(stderr, "ERROR: There are no commits!\n");
    return 1;
  }

  fprintf(stdout, "\n");
  while (limit--) {
    fprintf(stdout, "commit %s\n", commit_id);
    char commit_dir[MAX_LENGTH];
    sprintf(commit_dir, ".beargit/%s", commit_id);

    char msg_file_name[MAX_LENGTH];
    sprintf(msg_file_name, "%s/.msg", commit_dir);

    char msg[MSG_SIZE];
    read_string_from_file(msg_file_name, msg, MSG_SIZE);
    fprintf(stdout, "    %s\n\n", msg);

    char prev_file[MAX_LENGTH];
    sprintf(prev_file, "%s/.prev", commit_dir);

    read_string_from_file(prev_file, commit_id, COMMIT_ID_SIZE);
    if (strcmp(commit_id, "0000000000000000000000000000000000000000") == 0) {
      break;
    }
  }

  return 0;
}


const char* digits = "61c";

void next_commit_id(char* commit_id) {
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);

  // The first COMMIT_ID_BRANCH_BYTES=10 characters of the commit ID will
  // be used to encode the current branch number. This is necessary to avoid
  // duplicate IDs in different branches, as they can have the same pre-
  // decessor (so next_commit_id has to depend on something else).
  int n = get_branch_number(current_branch);
  for (int i = 0; i < COMMIT_ID_BRANCH_BYTES; i++) {
    commit_id[i] = digits[n%3];
    n /= 3;
  }

  // Use next_commit_id to fill in the rest of the commit ID.
  next_commit_id_part1(commit_id + COMMIT_ID_BRANCH_BYTES);
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");

  int branch_index = -1;
  int counter = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, branch_name) == 0) {
      branch_index = counter;
    }
    counter++;
  }

  fclose(fbranches);

  return branch_index;
}

/* beargit branch
 *
 * Beargit branch prints all the branches and puts a star in front of the current one. 
 * Do you remember beargit status? This is almost the same: you need to read the entire .branches file line by line and output it.
 * However, you also need to check each line against the string in .current_branch.
 * If they are the same, you need to print a * in front of it.
 * Note that we require you to print branches in the order of creation, from oldest to latest. 
 * Also note that if you have checked out a commit previously (in contrast to a branch), 
 * you are detached from the HEAD and don't have to print a star in front of any branch. 
 * This is even true if the commit you checked out is actually the HEAD of a branch.
 *
 * Output (to stdout):
 * This function should always return 0 (indicating success) and should never output to stderr.
 */

int beargit_branch() {
  /* COMPLETE THE REST */
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);
  
  FILE* fbranches = fopen(".beargit/.branches", "r");
  char line[BRANCHNAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, current_branch) == 0) {
      fprintf(stdout, "*");
    }
    fprintf(stdout, " %s\n", line);
  }
  fclose(fbranches);

  return 0;
}

/* beargit checkout
 *
 * Once you know whether you are dealing with a branch or a commit, you have to do one of two things:
 * If it's a commit, check out the commit by replacing the currently tracked files with those from the time of the commit.
 * If it's a branch (and you're not creating a new one), first check whether it exists. If yes, you need to switch to that branch. 
 * This means that you first store the latest commit of the current branch into the branch_branchname file, 
 * and then replace the content of current_branch by the new branch. You then read the branch_newbranch file to find out the HEAD commit of that branch, 
 * and then you check that commit out just like in 1). You are creating a new branch. 
 * This is very similar to 2), but you also have to add the branch to the .branches file and instead of reading the HEAD ID from .branch_branchname, 
 * you make the current prev ID the head ID for that branch and store it into that file.
 * Since we are nice people, we actually implemented the functionality above for you, except for the implementation of the actual checkout! 
 * But because we had to write this project in a rush, there are three mistakes in the beargit_checkout function 
 * -- you need to find and correct them for everything to run (one line per mistake). Consider using cgdb and printf for debugging to help you!
 * Note: The beargit_checkout function is taking two arguments: new_branch is true if and only if -b was supplied to the command, 
 * and arg contains the other command line argument.
 * 
 *
 * Output And Error:
 * If the argument is a commit ID (40 characters, each of which is '6', '1' or 'c') of a commit that exists, 
 * a branch that exists and new_branch is false, or a branch that doesn't exist and new_branch is true, 
 * the function should return 0 and produce no output on stderr.
 * If the argument is a commit ID but the commit does not exist, the function should return 1 and produce errors.
 * 
 * 
 * assignments: Find out 3 ERROR in beargit_checkout and complete checkout_commit(), is_it_a_commit_id();
 */

void delete_all_tracked_file_of_current_index() {
  FILE* findex = fopen(".beargit/.index", "r");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    fs_rm(line);
  }
  fclose(findex);
}

void copy_out_all_tracked_file(const char *commit_dir_name) {
  FILE* findex = fopen(".beargit/.index", "r");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");

    char file_name_in_commit_dir[MAX_LENGTH];
    sprintf(file_name_in_commit_dir, "%s/%s", commit_dir_name, line);  
    char file_name_in_init_dir[MAX_LENGTH];
    sprintf(file_name_in_init_dir, "%s", line);

    fs_cp(file_name_in_commit_dir, file_name_in_init_dir);
  }
  fclose(findex);
}

int checkout_commit(const char* commit_id) {
  /* COMPLETE THE REST */
  
  //Going through the index of the current index file, delete all those files 
  //(in the current directory; i.e., the directory where we ran beargit).
  delete_all_tracked_file_of_current_index();

  //In the special case that the new commit is the 00.0 commit
  if (strcmp(commit_id, "0000000000000000000000000000000000000000") == 0) {
    write_string_to_file(".beargit/.index", "");
    write_string_to_file(".beargit/.prev", commit_id);
    return 0;
  }

  //Copy the index from the commit that is being checked out to the .beargit directory.
  char commit_dir_name[MAX_LENGTH];
  sprintf(commit_dir_name, ".beargit/%s", commit_id);
  char new_index_file[MAX_LENGTH];
  sprintf(new_index_file, "%s/.index", commit_dir_name);
  fs_cp(new_index_file, ".beargit/.index");

  //Copy all that commit's tracked files from the commit's directory into the current directory.
  copy_out_all_tracked_file(commit_dir_name);

  write_string_to_file(".beargit/.prev", commit_id);

  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  /* COMPLETE THE REST */
  int id_length = strlen(commit_id);
  if (id_length != COMMIT_ID_BYTES)
    return 0;

  for (int i = 0; i < id_length; ++i) {
    char c = commit_id[i];
    if (c != '6' || c != '1' || c != 'c')
      return 0;
  }

  return 1;
}

int beargit_checkout(const char* arg, int new_branch) {
  // Get the current branch
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", current_branch, BRANCHNAME_SIZE);   // 1st ERROR at "current_branch"

  // If not detached, update the current branch by storing the current HEAD into that branch's file...
  // Even if we cancel later, this is still ok.
  if (strlen(current_branch)) {
    char current_branch_file[BRANCHNAME_SIZE+50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(".beargit/.prev", current_branch_file);
  }

  // Check whether the argument is a commit ID. If yes, we just stay in detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE] = ".beargit/";
    strcat(commit_dir, arg);
    if (!fs_check_dir_exists(commit_dir)) {
      fprintf(stderr, "ERROR: Commit %s does not exist\n", arg);
      return 1;
    }

    // Set the current branch to none (i.e., detached).
    write_string_to_file(".beargit/.current_branch", "");

    return checkout_commit(arg);
  }

  // Just a better name, since we now know the argument is a branch name.
  const char* branch_name = arg;

  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(branch_name) >= 0);

  // Check for errors.
  if (!(!branch_exists || !new_branch)) {
    fprintf(stderr, "ERROR: A branch named %s already exists\n", branch_name);
    return 1;
  } else if (!branch_exists && !new_branch) { // else if (!branch_exists && new_branch). 2nd ERROR at (new_branch)
    fprintf(stderr, "ERROR: No branch %s exists\n", branch_name);
    return 1;
  }

  // File for the branch we are changing into.
  //char* branch_file = ".beargit/.branch_"; // 3rd ERROR
  char branch_file[] = ".beargit/.branch_";
  strcat(branch_file, branch_name);

  // Update the branch file if new branch is created (now it can't go wrong anymore)
  if (new_branch) {
    FILE* fbranches = fopen(".beargit/.branches", "a");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    fs_cp(".beargit/.prev", branch_file); 
  }

  write_string_to_file(".beargit/.current_branch", branch_name); //check out branch_name

  // Read the head commit ID of this branch.
  char branch_head_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);

  // Check out the actual commit.
  return checkout_commit(branch_head_commit_id);
}

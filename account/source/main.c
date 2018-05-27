#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include <switch.h>

//This example shows how to get info for the current user account. See libnx acc.h.

int main(int argc, char **argv)
{
    Result rc=0;

    u128 userID=0;
    bool account_selected=0;
    AccountProfile profile;
    AccountUserData userdata;
    AccountProfileBase profilebase;

    char username[0x21];

    gfxInitDefault();
    consoleInit(NULL);

    memset(&userdata, 0, sizeof(userdata));
    memset(&profilebase, 0, sizeof(profilebase));

    rc = accountInitialize();
    if (R_FAILED(rc)) {
        printf("accountInitialize() failed: 0x%x\n", rc);
    }
	
    if (R_SUCCEEDED(rc)) {
		//Get the total number of user accounts
		s32 user_count = 0;
		rc = accountGetUserCount(&user_count);
		
		if (R_FAILED(rc)) {
            printf("accountGetUserCount() failed: 0x%x\n", rc);
        }
        else {
            printf("User count = %d\n", user_count);
        }
		
		
		//Get the user IDs of all users
		u128 *userIDs;
		userIDs = malloc(sizeof(u128) * ACC_USER_LIST_SIZE);
		
		rc = accountListAllUsers(userIDs);
		
		if (R_FAILED(rc)) {
            printf("accountListAllUsers() failed: 0x%x\n", rc);
        }
        else if (R_SUCCEEDED(rc)){
			// If there are less than 8 users created, then there will be some empty IDs 
			// so we only loop for the number of users that we counted previously
            for(int i = 0; i < user_count; i++){
				printf("Found UserId: 0x%lx 0x%lx\n", (u64)(userIDs[i] >> 64), (u64)userIDs[i]);
			}
        }
		
        rc = accountGetActiveUser(&userID, &account_selected);

        if (R_FAILED(rc)) {
            printf("accountGetActiveUser() failed: 0x%x\n", rc);
        }
        else if(!account_selected) {
            printf("No user is currently selected.\n");
            rc = -1;
        }

        if (R_SUCCEEDED(rc)) {
            printf("Current userID: 0x%lx 0x%lx\n", (u64)(userID>>64), (u64)userID);

            rc = accountGetProfile(&profile, userID);

            if (R_FAILED(rc)) {
                printf("accountGetProfile() failed: 0x%x\n", rc);
            }
        }

        if (R_SUCCEEDED(rc)) {
            rc = accountProfileGet(&profile, &userdata, &profilebase);//userdata is otional, see libnx acc.h.

            if (R_FAILED(rc)) {
                printf("accountProfileGet() failed: 0x%x\n", rc);
            }

            if (R_SUCCEEDED(rc)) {
                memset(username,  0, sizeof(username));
                strncpy(username, profilebase.username, sizeof(username)-1);//Even though profilebase.username usually has a NUL-terminator, don't assume it does for safety.

                printf("Username: %s\n", username);//Note that the print-console doesn't support UTF-8. The username is UTF-8, so this will only display properly if there isn't any non-ASCII characters. To display it properly, a print method which supports UTF-8 should be used instead.

                //You can also use accountProfileGetImageSize()/accountProfileLoadImage() to load the icon for use with a JPEG library, for displaying the user profile icon. See libnx acc.h.
            }

            accountProfileClose(&profile);
        }

        accountExit();
    }

    // Main loop
    while(appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }

    gfxExit();
    return 0;
}


const functions = require("firebase-functions/v1");
const admin = require("firebase-admin");

// Initialize Firebase Admin SDK
admin.initializeApp();


// Function to send notifications based on different modes
exports.sendNotification = functions
  .region('europe-west1') // Ensure the region matches your database location
  .database
  .instance('smart-doorbell-2025-default-rtdb')
  .ref('/notification')
  .onUpdate((change, context) => {
    const newValue = change.after.val();

    if (newValue !== "NONE") {
      let title = "";
      let body = "";

      // Determine the notification type and set title and body accordingly
      switch (newValue) {
        case "Security Alert!":
          title = "Security Alert!";
          body = "A security alert was triggered!";
          break;
        case "Delivery on the Door":
          title = "Delivery Alert!";
          body = "A delivery is waiting at the door.";
          break;
        case "The Door is ringing!":
          title = "Doorbell Alert!";
          body = "Someone is at the door.";
          break;
        case "Motion Detected!":
          title = "Motion Detected!";
          body = "Motion was detected near the door. Look at the camera.";
          break;
        default:
          console.log("Unknown notification type:", newValue);
          return null; // Exit if the notification type is unknown
      }

      const message = {
        notification: {
          title: title,
          body: body,
        },
        topic: "alerts", // Specify the topic for FCM
      };

      return admin
        .messaging()
        .send(message)
        .then((response) => {
          console.log(`${title} notification sent successfully:`, response);
          return null;
        })
        .catch((error) => {
          console.error(`Error sending ${title} notification:`, error);
        });
    }

    return null;
  });


// Function to send Main Password Change notifications
exports.sendPasswordChangeNotification = functions
  .region('europe-west1') // Ensure the region matches your database location
  .database
  .instance('smart-doorbell-2025-default-rtdb')
  .ref('/main_password')
  .onUpdate(async (change, context) => {
    const newPassword = change.after.val();

    try {
      // Fetch the user info from the "password_update_info" node
      const infoSnapshot = await admin.database().ref('/password_update_info').get();
      const info = infoSnapshot.val();

      if (info) {
        const userName = info.name;
        const message = {
          notification: {
            title: "Main Password Changed!!",
            body: `${userName} changed the password to: ${newPassword}`,
          },
          topic: "alerts", // Send to all users subscribed to "alerts"
        };

        // Send the notification
        await admin.messaging().send(message);
        console.log('Main password change notification sent successfully');
      }
    } catch (error) {
      console.error('Error sending main password change notification:', error);
    }

    return null;
  });

// Function to send Temp Password Addition notifications
exports.notifyTempPasswordAdded = functions
  .region('europe-west1') // Ensure it matches your database region
  .database
  .instance('smart-doorbell-2025-default-rtdb')
  .ref('/temp_password/{pushId}')
  .onCreate((snapshot, context) => {
    const tempPasswordData = snapshot.val();

    const userName = tempPasswordData.name || "Someone";
    const tempPassword = tempPasswordData.password || "Unknown";
    const expiresAt = tempPasswordData.expires_at || "Unknown date";

    const message = {
      notification: {
        title: "New Temporary Password Added!",
        body: `${userName} added a temporary password: ${tempPassword} (Expires: ${expiresAt})`,
      },
      topic: "alerts", // Notify all users subscribed to the topic
    };

    return admin
      .messaging()
      .send(message)
      .then((response) => {
        console.log("Temporary password notification sent successfully:", response);
        return null;
      })
      .catch((error) => {
        console.error("Error sending temporary password notification:", error);
      });
  });

  // Function to send Temp Password Deletion notifications
exports.notifyTempPasswordDeleted = functions
  .region('europe-west1')
  .database
  .instance('smart-doorbell-2025-default-rtdb')
  .ref('/temp_password/{pushId}')
  .onDelete((snapshot, context) => {
    const tempPasswordData = snapshot.val();
    const userName = tempPasswordData.name || "Someone";
    const tempPassword = tempPasswordData.password || "Unknown";

    const message = {
        notification: {
          title: "Temporary Password Deleted",
          body: `The temporary password: ${tempPassword}, added by ${userName}, has been deleted.`,
        },
        topic: "alerts",
      };

    return admin
      .messaging()
      .send(message)
      .then((response) => {
        console.log("Temporary password deletion notification sent successfully:", response);
        return null;
      })
      .catch((error) => {
        console.error("Error sending deletion notification:", error);
      });
  });
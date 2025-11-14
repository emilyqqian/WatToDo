import React from 'react';

function Profile({ user }) {
  return (
    
    <div className="profile-page">
      <h2>Profile</h2>
      <p>
        <strong>Username:</strong> {user.username}
      </p>
      <p>
        <strong>User ID:</strong> {user.userId}
      </p>
      <p>
        <strong>XP:</strong> {user.xp}
      </p>
      <p>
        <strong>Password (hashed):</strong> {user.passwordHash}
      </p>
    </div>
  );
}

export default Profile;
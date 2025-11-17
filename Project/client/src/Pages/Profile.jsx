import { useGlobal } from "../SessionManager";
import { Navigate } from 'react-router-dom';

function Profile() {
  const { state } = useGlobal();

  if (!state.loggedIn) {
      return (
        <Navigate to="/login" replace />
      )
  }

  return (
    <div className="profile-page">
      <h2>Profile</h2>
      <p>
        <strong>Username:</strong> {state.user.username}
      </p>
      <p>
        <strong>User ID:</strong> {state.user.userId}
      </p>
      <p>
        <strong>XP:</strong> {state.user.xp}
      </p>
    </div>
  );
}

export default Profile;
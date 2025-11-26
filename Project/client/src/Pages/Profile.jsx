import { useGlobal } from "../SessionManager";
import { useState } from 'react'
import { Navigate } from 'react-router-dom';
import SetPassword from "../Components/SetPassword";

import {
  Container,
  Button,
  Box
} from '@mui/material'
import SetUsername from "../Components/SetUsername";
import { logout } from "../APIManager";
import { useNavigate } from 'react-router-dom';

function Profile() {
  const { state, updateState } = useGlobal();

  const [isUsernamePanelOpen, setUsernamePanelOpen] = useState(false)
  const [isPasswordPanelOpen, setPasswordPanelOpen] = useState(false)

  const navigator = useNavigate();

  if (!state.loggedIn) {
      return (
        <Navigate to="/login" replace />
      )
  }

  function onLogOut(){
    logout(state.user.userId);
    updateState({ loggedIn: false, user: null })
    navigator('/')
  }

  return (
    <Box sx={{
        minHeight: '100vh',
        background: `linear-gradient(135deg, rgba(102, 126, 234, 0.1), rgba(118, 75, 162, 0.1)), url('/background.jpg')`,
        backgroundSize: 'cover',
        backgroundPosition: 'center',
        backgroundAttachment: 'fixed',
        py: 0,
        px: 2,
        pl: '10%'
      }}>
      <h1>My Profile: </h1>
      <p>
        <strong>Username:</strong> {state.user.username}
        <Button
            variant="contained"
            size="medium"
            onClick={() => {
              setPasswordPanelOpen(false)
              setUsernamePanelOpen(true)
            }}
            sx={{ alignSelf: { xs: 'stretch', md: 'center' }, ml: 5 }}
          >
            Edit Username
          </Button>
      </p>
      <p>
        <strong>User ID:</strong> {state.user.userId}
      </p>
      <p>
        <strong>XP:</strong> {state.user.xp_points}
      </p>

      <Button
            variant="contained"
            size="medium"
            onClick={() => {
              setPasswordPanelOpen(true)
              setUsernamePanelOpen(false)
            }}
            sx={{ alignSelf: { xs: 'stretch', md: 'center' } }}
          >
            Change Password
      </Button>


      <SetPassword
        open={isPasswordPanelOpen}
        onClose={() => setPasswordPanelOpen(false)}
      />

      <SetUsername
        username={state.user.username}
        open={isUsernamePanelOpen}
        onClose={() => setUsernamePanelOpen(false)}
      />

      <p>
        <Button
          variant="contained"
          size="medium"
          onClick={onLogOut}
          sx={{ alignSelf: { xs: 'stretch', md: 'center' } }}
        >
          Logout
        </Button>
      </p>
    </Box>
  )
}

export default Profile;
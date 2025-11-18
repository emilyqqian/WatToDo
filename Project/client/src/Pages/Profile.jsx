import { useGlobal } from "../SessionManager";
import { useState } from 'react'
import { Navigate } from 'react-router-dom';
import SetPassword from "../Components/SetPassword";

import {
  Container,
  Button
} from '@mui/material'
import SetUsername from "../Components/SetUsername";

function Profile() {
  const { state } = useGlobal();

  const [isUsernamePanelOpen, setUsernamePanelOpen] = useState(false)
  const [isPasswordPanelOpen, setPasswordPanelOpen] = useState(false)

  if (!state.loggedIn) {
      return (
        <Navigate to="/login" replace />
      )
  }

  return (
    <Container spacing={10} sx={{ml: '10%'}}>
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
        <strong>XP:</strong> {state.user.xp}
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
    </Container>
  )
}

export default Profile;
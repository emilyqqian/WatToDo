import {
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  TextField,
} from '@mui/material'

import { useState } from 'react'
import { useGlobal } from '../SessionManager';
import { updateUser } from '../APIManager';

function SetUsername({username, open, onClose}) {
    const [password, setPassword] = useState(username)
    const { state, updateState } = useGlobal();
  

  const handleClose = () => {
    onClose()
    setPassword('')
  }

  const handleSave = () => {
        let instance = {
            username: password,
            userId: state.user.userId,
            password: state.user.password,
            xp: state.user.xp,
            xp_points: state.user.xp
        }

        updateUser(instance, state.user.userId).then(data => {
            if (data) {
              updateState({ user: instance, loggedIn: true})
              handleClose()
            }
        })
        return;
  }

  return (
    <Dialog open={open} onClose={handleClose} fullWidth maxWidth="sm">
      <DialogTitle>Change  Username:</DialogTitle>
      <DialogContent sx={{mt: '3%'}}>
        <TextField
            label="Username"
            value={password}
            onChange={(event) => setPassword(event.target.value)}
            fullWidth
            sx={{mb: '5%'}}
        />
      </DialogContent>
      <DialogActions sx={{ px: 3, pb: 2 }}>
        <Button onClick={handleClose}>Cancel</Button>
        <Button variant="contained" onClick={handleSave}>
          Save Username
        </Button>
      </DialogActions>
    </Dialog>
  )
}

export default SetUsername;